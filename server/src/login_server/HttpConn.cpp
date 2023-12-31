/*
 * HttpConn.cpp
 *
 *  Created on: 2013-9-29
 *      Author: ziteng@mogujie.com
 */

#include "HttpConn.h"
#include "json/json.h"
#include "LoginConn.h"
#include "HttpParserWrapper.h"
#include "ipparser.h"

// key 是一个能唯一标识每个进入的 http 连接的数字，不是 socket handle，由于 socket 复用 socket handle 并不能唯一标识每个 http 连接
// value 是连接对象
static HttpConnMap_t g_http_conn_map;

// msg_server 的信息。多个 msg_server 用于负载均衡
// 在 LoginConn.cpp 中定义
extern map<uint32_t, msg_serv_info_t*> g_msg_serv_info;

// 提供对一个字符串是否是点分10进制ip地址的简单判断
// 在 login_server.cpp 中定义
extern IpParser* pIpParser;

// 图片服务器地址
// 在 login_server.cpp 中定义
extern string strMsfsUrl;

// 服务发现地址
// 在 login_server.cpp 中定义
extern string strDiscovery;

// conn_handle 从0开始递增，可以防止因 socket handle 重用引起的一些冲突
// 在每一个连接对象的构造函数中实现该数值的加1
static uint32_t g_conn_handle_generator = 0;

CHttpConn* FindHttpConnByHandle(uint32_t conn_handle)
{
    CHttpConn* pConn = NULL;
    HttpConnMap_t::iterator it = g_http_conn_map.find(conn_handle);
    if (it != g_http_conn_map.end())
    {
        pConn = it->second;
    }

    return pConn;
}

void httpconn_callback(void* callback_data, uint8_t msg, uint32_t handle, uint32_t uParam, void* pParam)
{
	NOTUSED_ARG(uParam);
	NOTUSED_ARG(pParam);

	// convert void* to uint32_t, oops
	uint32_t conn_handle = *((uint32_t*)(&callback_data));
    CHttpConn* pConn = FindHttpConnByHandle(conn_handle);
    if (!pConn)
    {
        return;
    }

	switch (msg)
	{
	case NETLIB_MSG_READ:
		pConn->OnRead();
		break;
	case NETLIB_MSG_WRITE:
		pConn->OnWrite();
		break;
	case NETLIB_MSG_CLOSE:
		pConn->OnClose();
		break;
	default:
		log("!!!httpconn_callback error msg: %d ", msg);
		break;
	}
}

void http_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	CHttpConn* pConn = NULL;
	HttpConnMap_t::iterator it, it_old;
	uint64_t cur_time = get_tick_count();

	for (it = g_http_conn_map.begin(); it != g_http_conn_map.end();)
	{
		it_old = it;
		it++;

		pConn = it_old->second;
		pConn->OnTimer(cur_time);
	}
}

void init_http_conn()
{
	netlib_register_timer(http_conn_timer_callback, NULL, 1000);
}

CHttpConn::CHttpConn()
{
	m_busy = false;
	m_sock_handle = NETLIB_INVALID_HANDLE;
    m_state = CONN_STATE_IDLE;
    
	m_last_send_tick = m_last_recv_tick = get_tick_count();
	m_conn_handle = ++g_conn_handle_generator;
	if (m_conn_handle == 0)
	{
		m_conn_handle = ++g_conn_handle_generator;
	}

	//log("CHttpConn, handle=%u\n", m_conn_handle);
}

CHttpConn::~CHttpConn()
{
	//log("~CHttpConn, handle=%u\n", m_conn_handle);
}

int CHttpConn::Send(void* data, int len)
{
	m_last_send_tick = get_tick_count();

	if (m_busy)
	{
		m_out_buf.Write(data, len);
		return len;
	}

	int ret = netlib_send(m_sock_handle, data, len);
	if (ret < 0)
    {
        ret = 0;
    }

	// 只发送了部分数据，需要将未发送完成的数据放入发送缓冲区
	if (ret < len)
	{
		m_out_buf.Write((char*)data + ret, len - ret);
		m_busy = true;
		//log("not send all, remain=%d\n", m_out_buf.GetWriteOffset());
	}
    else
    {
        OnWriteComlete();
    }

	return len;
}

void CHttpConn::Close()
{
    m_state = CONN_STATE_CLOSED;
    
    g_http_conn_map.erase(m_conn_handle);
    netlib_close(m_sock_handle);

    ReleaseRef();
}

void CHttpConn::OnConnect(net_handle_t handle)
{
    printf("OnConnect, handle=%d\n", handle);
    m_sock_handle = handle;
    m_state = CONN_STATE_CONNECTED;
    // m_conn_handle 在 CHttpConn 构造函数执行时就完成了增加
    g_http_conn_map.insert(make_pair(m_conn_handle, this));
    
    netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)httpconn_callback);
    netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, reinterpret_cast<void *>(m_conn_handle) );
    netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&m_peer_ip);
}

void CHttpConn::OnRead()
{
	for (;;)
	{
		uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
		if (free_buf_len < READ_BUF_SIZE + 1)
        {
            m_in_buf.Extend(READ_BUF_SIZE + 1);
        }

		int ret = netlib_recv(m_sock_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
		if (ret <= 0)
        {
            break;
        }

		m_in_buf.IncWriteOffset(ret);

		m_last_recv_tick = get_tick_count();
	}

	// 每次请求对应一个 HTTP 连接，所以读完数据后，不用在同一个连接里面准备读取下个请求（因为这是 HTTP 请求）
	char* in_buf = (char*)m_in_buf.GetBuffer();
	uint32_t buf_len = m_in_buf.GetWriteOffset();
	in_buf[buf_len] = '\0';
    
    // 如果 buf_len 过长可能是受到攻击，则断开连接
    // 正常的 url 最大长度为2048，我们接受的所有数据长度不得大于1K
    if (buf_len > 1024)
    {
        log("get too much data:%s ", in_buf);
        Close();
        return;
    }

	//log("OnRead, buf_len=%u, conn_handle=%u\n", buf_len, m_conn_handle); // for debug
	// 解析 Http 请求
	m_cHttpParser.ParseHttpContent(in_buf, buf_len);

	if (m_cHttpParser.IsReadAll())
	{
		string url = m_cHttpParser.GetUrl();
		// 只接收 url 为 /msg_server 的请求
		if (strncmp(url.c_str(), "/msg_server", 11) == 0)
		{
            string content = m_cHttpParser.GetBodyContent();
            _HandleMsgServRequest(url, content);
		} else {
			log("url unknown, url=%s ", url.c_str());
			Close();
		}
	}
}

void CHttpConn::OnWrite()
{
	if (!m_busy)
    {
        return;
    }

	int ret = netlib_send(m_sock_handle, m_out_buf.GetBuffer(), m_out_buf.GetWriteOffset());
	if (ret < 0)
    {
        ret = 0;
    }

	int out_buf_size = (int)m_out_buf.GetWriteOffset();

	m_out_buf.Read(NULL, ret);

	if (ret < out_buf_size)
	{
		m_busy = true;
		log("not send all, remain=%d ", m_out_buf.GetWriteOffset());
	}
	else
	{
        OnWriteComlete();
		m_busy = false;
	}
}

void CHttpConn::OnClose()
{
    Close();
}

void CHttpConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_recv_tick + HTTP_CONN_TIMEOUT)
	{
		log("HttpConn timeout, handle=%d ", m_conn_handle);
		Close();
	}
}

// Add By Lanhu 2014-12-19 通过登陆IP来优选电信还是联通IP
void CHttpConn::_HandleMsgServRequest(string& url, string& post_data)
{
    msg_serv_info_t* pMsgServInfo;
    uint32_t min_user_cnt = (uint32_t)-1;
    map<uint32_t, msg_serv_info_t*>::iterator it_min_conn = g_msg_serv_info.end();
    map<uint32_t, msg_serv_info_t*>::iterator it;
    if (g_msg_serv_info.size() <= 0)
    {
        Json::Value value;
        value["code"] = 1;
        value["msg"] = "没有msg_server";
        string strContent = value.toStyledString();
        char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
        Send((void*)szContent, strlen(szContent));
        return ;
    }

    // 简单的负载均衡逻辑：
    // 找到连接数未满且连接数量最小的 msgserver
    for (it = g_msg_serv_info.begin(); it != g_msg_serv_info.end(); it++)
    {
        pMsgServInfo = it->second;
        if ((pMsgServInfo->cur_conn_cnt < pMsgServInfo->max_conn_cnt) && (pMsgServInfo->cur_conn_cnt < min_user_cnt))
        {
            it_min_conn = it;
            min_user_cnt = pMsgServInfo->cur_conn_cnt;
        }
    }
    
    if (it_min_conn == g_msg_serv_info.end())
    {
        log("All TCP MsgServer are full ");
        Json::Value value;
        value["code"] = 2;
        value["msg"] = "负载过高";
        string strContent = value.toStyledString();
        char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
        Send((void*)szContent, strlen(szContent));
        return;
    }
    else
    {
        Json::Value value;
        value["code"] = 0;
        value["msg"] = "";
        if (pIpParser->isTelcome(GetPeerIP()))
        {
            value["priorIP"] = string(it_min_conn->second->ip_addr1);
            value["backupIP"] = string(it_min_conn->second->ip_addr2);
            value["msfsPrior"] = strMsfsUrl;
            value["msfsBackup"] = strMsfsUrl;
        }
        else
        {
            value["priorIP"] = string(it_min_conn->second->ip_addr2);
            value["backupIP"] = string(it_min_conn->second->ip_addr1);
            value["msfsPrior"] = strMsfsUrl;
            value["msfsBackup"] = strMsfsUrl;
        }
        value["discovery"] = strDiscovery;
        value["port"] = int2string(it_min_conn->second->port);
        string strContent = value.toStyledString();
        char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        uint32_t nLen = strContent.length();
        snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, nLen, strContent.c_str());
        Send((void*)szContent, strlen(szContent));
        return;
    }
}

void CHttpConn::OnWriteComlete()
{
    log("write complete ");
    Close();
}

