/*
 * ProxyConn.cpp
 *
 *  Created on: 2014年7月25日
 *      Author: ziteng
 */

#include "ProxyConn.h"
#include "ProxyTask.h"
#include "HandlerMap.h"
#include "atomic.h"
#include "IM.Other.pb.h"
#include "IM.BaseDefine.pb.h"
#include "IM.Server.pb.h"
#include "ThreadPool.h"
#include "SyncCenter.h"


// 每一个建立的 socket 连接都保存在这里，以 handle 为key
static ConnMap_t g_proxy_conn_map;
// 由于 socket 复用，g_proxy_conn_map 无法唯一标识每个进入的消息，故增加此 map，以 uuid 为 key，以消息为 value
static UserMap_t g_uuid_conn_map;
// 全局的，各种消息类型的处理函数
static CHandlerMap* s_handler_map;
// g_uuid_conn_map 中上一个 uuid
uint32_t CProxyConn::s_uuid_alloctor = 0;
// 保护主线程响应消息队列的锁
CLock CProxyConn::s_list_lock;
// 主线程用于响应消息的响应消息队列
list<ResponsePdu_t*> CProxyConn::s_response_pdu_list;
// 用于处理消息任务的线程池
static CThreadPool g_thread_pool;

// 按时调用此方法，此方法在内部调用所有连接（以handle为标识）的 OnTimer 方法，用于实现心跳
void proxy_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	uint64_t cur_time = get_tick_count();
	for (ConnMap_t::iterator it = g_proxy_conn_map.begin(); it != g_proxy_conn_map.end(); )
	{
		ConnMap_t::iterator it_old = it;
		it++;

		CProxyConn* pConn = (CProxyConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}
}

// 主线程的循环需要调用此方法，用于向客户端发回响应
void proxy_loop_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	CProxyConn::SendResponsePduList();
}

/*
 * 用于优雅的关闭连接：
 * 服务器收到 SIGTERM 信号后，发送 CImPduStopReceivePacket 数据包给每个代理连接，通知消息服务器不要往自己发送数据包请求，
 * 然后注册4s后调用本函数（退出代理连接进程）
 */
void exit_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	log("exit_callback...");
	exit(0);
}

// 处理 SIGTERM 信号，发送 CImPduStopReceivePacket 数据包给每个代理连接，通知消息服务器不要往自己发送数据包请求，然后在4秒后关掉进程
static void sig_handler(int sig_no)
{
	if (sig_no == SIGTERM)
	{
		log("receive SIGTERM, prepare for exit");
        CImPdu cPdu;
        IM::Server::IMStopReceivePacket msg;
        msg.set_result(0);
        cPdu.SetPBMsg(&msg);
        cPdu.SetServiceId(IM::BaseDefine::SID_OTHER);
        cPdu.SetCommandId(IM::BaseDefine::CID_OTHER_STOP_RECV_PACKET);
        for (ConnMap_t::iterator it = g_proxy_conn_map.begin(); it != g_proxy_conn_map.end(); it++)
        {
            CProxyConn* pConn = (CProxyConn*)it->second;
            pConn->SendPdu(&cPdu);
        }

        // 停掉代理连接进程之前，先停止同步服务
        CSyncCenter::getInstance()->stopSync();

		netlib_register_timer(exit_callback, NULL, 4000);
	}
}

int init_proxy_conn(uint32_t thread_num)
{
	s_handler_map = CHandlerMap::getInstance();
	g_thread_pool.Init(thread_num);

	netlib_add_loop(proxy_loop_callback, NULL);

	signal(SIGTERM, sig_handler);

	return netlib_register_timer(proxy_timer_callback, NULL, 1000);
}

CProxyConn* get_proxy_conn_by_uuid(uint32_t uuid)
{
	CProxyConn* pConn = NULL;
	UserMap_t::iterator it = g_uuid_conn_map.find(uuid);
	if (it != g_uuid_conn_map.end())
	{
		pConn = (CProxyConn *)it->second;
	}

	return pConn;
}

CProxyConn::CProxyConn()
{
	m_uuid = ++CProxyConn::s_uuid_alloctor;
	if (m_uuid == 0)
	{
		m_uuid = ++CProxyConn::s_uuid_alloctor;
	}

	g_uuid_conn_map.insert(make_pair(m_uuid, this));
}

CProxyConn::~CProxyConn()
{

}

void CProxyConn::Close()
{
	if (m_handle != NETLIB_INVALID_HANDLE)
	{
		netlib_close(m_handle);
		g_proxy_conn_map.erase(m_handle);

		g_uuid_conn_map.erase(m_uuid);
	}

	ReleaseRef();
}

void CProxyConn::OnConnect(net_handle_t handle)
{
	m_handle = handle;

	g_proxy_conn_map.insert(make_pair(handle, this));

	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_proxy_conn_map);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&m_peer_ip);
	netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&m_peer_port);

	log("connect from %s:%d, handle=%d", m_peer_ip.c_str(), m_peer_port, m_handle);
}

// 由于数据包是在另一个线程处理的，所以不能在主线程delete数据包，所以需要Override这个方法
void CProxyConn::OnRead()
{
	for (;;) {
		uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
		if (free_buf_len < READ_BUF_SIZE)
			m_in_buf.Extend(READ_BUF_SIZE);

		int ret = netlib_recv(m_handle, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(), READ_BUF_SIZE);
		if (ret <= 0)
			break;

		m_recv_bytes += ret;
		m_in_buf.IncWriteOffset(ret);
		m_last_recv_tick = get_tick_count();
	}

	uint32_t pdu_len = 0;
    try {
        while ( CImPdu::IsPduAvailable(m_in_buf.GetBuffer(), m_in_buf.GetWriteOffset(), pdu_len) ) {
            HandlePduBuf(m_in_buf.GetBuffer(), pdu_len);
            m_in_buf.Read(NULL, pdu_len);
        }
    } catch (CPduException& ex) {
        log("!!!catch exception, err_code=%u, err_msg=%s, close the connection ",
            ex.GetErrorCode(), ex.GetErrorMsg());
        OnClose();
    }
	
}


void CProxyConn::OnClose()
{
	Close();
}

void CProxyConn::OnTimer(uint64_t curr_tick)
{
    // 这种实现即使是检测到超时了，也还是会向客户端发送一次心跳，然后再关闭连接
	if (curr_tick > m_last_send_tick + SERVER_HEARTBEAT_INTERVAL)
	{
        CImPdu cPdu;
        IM::Other::IMHeartBeat msg;
        cPdu.SetPBMsg(&msg);
        cPdu.SetServiceId(IM::BaseDefine::SID_OTHER);
        cPdu.SetCommandId(IM::BaseDefine::CID_OTHER_HEARTBEAT);
		SendPdu(&cPdu);
	}

	if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT)
	{
		log("proxy connection timeout %s:%d", m_peer_ip.c_str(), m_peer_port);
		Close();
	}
}

void CProxyConn::HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len)
{
    CImPdu* pPdu = NULL;
    pPdu = CImPdu::ReadPdu(pdu_buf, pdu_len);
    
    if (pPdu->GetCommandId() == IM::BaseDefine::CID_OTHER_HEARTBEAT)
    {
        return;
    }
    
    pdu_handler_t handler = s_handler_map->GetHandler(pPdu->GetCommandId());
    
    if (handler)
    {
        CTask* pTask = new CProxyTask(m_uuid, handler, pPdu);
        g_thread_pool.AddTask(pTask);
    }
    else
    {
        log("no handler for packet type: %d", pPdu->GetCommandId());
    }
}

void CProxyConn::AddResponsePdu(uint32_t conn_uuid, CImPdu* pPdu)
{
	ResponsePdu_t* pResp = new ResponsePdu_t;
	pResp->conn_uuid = conn_uuid;
	pResp->pPdu = pPdu;

	s_list_lock.lock();
	s_response_pdu_list.push_back(pResp);
	s_list_lock.unlock();
}

void CProxyConn::SendResponsePduList()
{
	s_list_lock.lock();
	while (!s_response_pdu_list.empty()) {
		ResponsePdu_t* pResp = s_response_pdu_list.front();
		s_response_pdu_list.pop_front();
		s_list_lock.unlock();

		CProxyConn* pConn = get_proxy_conn_by_uuid(pResp->conn_uuid);
		if (pConn) {
			if (pResp->pPdu) {
			    log("send puc to peer: %d", pResp->conn_uuid);
				pConn->SendPdu(pResp->pPdu);
			} else {
				log("close connection uuid=%d by parse pdu error\b", pResp->conn_uuid);
				pConn->Close();
			}
		}

		if (pResp->pPdu)
			delete pResp->pPdu;
		delete pResp;

		s_list_lock.lock();
	}

	s_list_lock.unlock();
}
