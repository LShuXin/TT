# 服务器端编程心得（三）—— 一个服务器程序的架构介绍

> 原文： https://balloonwj.blog.csdn.net/article/details/53426777


本文将介绍我曾经做过的一个项目的服务器架构和服务器编程的一些重要细节。

## 一、程序运行环境

- 操作系统：centos 7.0

- 编译器：gcc/g++ 4.8.3     cmake 2.8.11

- mysql 数据库：5.5.47

- 项目代码管理工具：VS2013



## 二、程序结构

该程序总共有17个线程，其中分为9个数据库工作线程D和一个日志线程L，6个普通工作线程W，一个主线程M。（以下会用这些字母来代指这些线程）

### （一）数据库工作线程的用途

9个数据库工作线程在线程启动之初，与 mysql 建立连接，也就是说每个线程都与 mysql 保持一路连接，共9个数据库连接。

每个数据库工作线程同时存在两个任务队列，第一个队列A存放需要执行数据库增删查改操作的任务sqlTask，第二个队列B存放sqlTask执行完成后的结果。sqlTask 执行完成后立即放入结果队列中，因而结果队列中任务也是一个个的需要执行的任务。大致伪代码如下：

	void db_thread_func()
	{
	    while (!m_bExit)
	    {
	        if (NULL != (pTask = m_sqlTask.Pop()))
	        {
	            // 从m_sqlTask中取出的任务先执行完成后，pTask将携带结果数据
	            pTask->Execute();			
	            // 得到结果后，立刻将该任务放入结果任务队列
	            m_resultTask.Push(pTask);
	            continue;
	        }
	        sleep(1000);
	    }
	}
现在的问题来了：
1. 任务队列A中的任务从何而来，目前只有消费者，没有生产者，那么生产者是谁？

2. 任务队列B中的任务将去何方，目前只有生产者没有消费者。

这两个问题先放一会儿，等到后面我再来回答。



### （二）工作线程和主线程

在介绍主线程和工作线程具体做什么时，我们介绍下服务器编程中常常抽象出来的几个概念（这里以tcp连接为例）：

1.  TcpServer 即 Tcp 服务，服务器需要绑定 ip 地址和端口号，并在该端口号上侦听客户端的连接（往往由一个成员变量 TcpListener 来管理侦听细节）。所以一个 TcpServer 要做的就是这些工作。除此之外，每当有新连接到来时，TcpServer 需要接收新连接，当多个新连接存在时，TcpServer 需要有条不紊地管理这些连接：连接的建立、断开等，即产生和管理下文中说的 TcpConnection 对象。
1.  一个连接对应一个 TcpConnection 对象，TcpConnection 对象管理着这个连接的一些信息：如连接状态、本端和对端的 ip 地址和端口号等。
1.  数据通道对象 Channel，Channel 记录了 socket 的句柄，因而是一个连接上执行数据收发的真正执行者，Channel 对象一般作为 TcpConnection 的成员变量。

4. TcpSession 对象，是将 Channel 收取的数据进行解包，或者对准备好的数据进行装包，并传给 Channel 发送。

归纳起来：一个 TcpServer 依靠 TcpListener 对新连接的侦听和处理，依靠 TcpConnection 对象对连接上的数据进行管理，TcpConnection 实际依靠 Channel 对数据进行收发，依靠 TcpSession 对数据进行装包和解包。也就是说一个 TcpServer 存在一个 TcpListener，对应多个 TcpConnection，有几个 TcpConnection 就有几个 TcpSession，同时也就有几个 Channel。

以上说的 TcpServer、TcpListener、TcpConnection、Channel 和 TcpSession 是服务器框架的网络层。一个好的网络框架，应该做到与业务代码脱耦。即上层代码只需要拿到数据，执行业务逻辑，而不用关注数据的收发和网络数据包的封包和解包以及网络状态的变化（比如网络断开与重连）。



拿数据的发送来说：

当业务逻辑将数据交给 TcpSession，TcpSession 将数据装好包后（装包过程后可以有一些加密或压缩操作），交给 TcpConnection::SendData()，而TcpConnection::SendData() 实际是调用 Channel::SendData()，因为 Channel 含有 socket 句柄，所以 Channel::SendData() 真正调用 send()/sendto()/write() 方法将数据发出去。



对于数据的接收，稍微有一点不同：

通过 select()/poll()/epoll() 等 IO multiplex 技术，确定好了哪些 TcpConnection 上有数据到来后，激活该 TcpConnection 的 Channel 对象去调用recv()/recvfrom()/read() 来收取数据。数据收到以后，将数据交由 TcpSession 来处理，最终交给业务层。注意数据收取、解包乃至交给业务层是一定要分开的。我的意思是：最好不要把"数据收取" & ""解包交给业务层"这两部分的逻辑放在一起。因为数据收取是IO操作，而解包和交给业务层是逻辑计算操作。IO操作一般比逻辑计算要慢。到底如何安排要根据服务器业务来取舍，也就是说你要想好你的服务器程序的性能瓶颈在网络IO还是逻辑计算，即使是网络IO，也可以分为上行操作和下行操作，上行操作即客户端发数据给服务器，下行即服务器发数据给客户端。有时候数据上行少，下行大。（如游戏服务器，一个npc移动了位置，上行是该客户端通知服务器自己最新位置，而下行确是服务器要告诉在场的每个客户端）。

在我的博文《服务器端编程心得（一）—— 主线程与工作线程的分工》中介绍了，工作线程的流程：

    while (!m_bQuit)
    {
        epoll_or_select_func();
        handle_io_events();
        handle_other_things();
    }

其中 epoll_or_select_func() 即是上文所说的通过 select()/poll()/epoll() 等 IO multiplex 技术，确定好了哪些 TcpConnection 上有数据到来。我的服务器代码中一般只会监测 socket 可读事件，而不会监测 socket 可写事件。至于如何发数据，文章后面会介绍。所以对于可读事件，以 epoll 为例，这里需要设置的标识位是：

- EPOLLIN 普通可读事件（当连接正常时，产生这个事件，recv()/read()函数返回收到的字节数；当连接关闭，这两个函数返回0，也就是说我们设置这个标识已经可以监测到新来数据和对端关闭事件）

- EPOLLRDHUP 对端关闭事件（linux man手册上说这个事件可以监测对端关闭，但我实际调试时发送即使对端关闭也没触发这个事件，仍然是EPOLLIN，只不过此时调用recv()/read()函数，返回值会为0，所以实际项目中是否可以通过设置这个标识来监测对端关闭，仍然待考证）

- EPOLLPRI 带外数据

  muduo 里面将 epoll_wait 的超时时间设置为1毫秒，我的另一个项目将 epoll_wait 超时时间设置为10毫秒。这两个数值供大家参考。

这个项目中，工作线程和主线程都是上文代码中的逻辑，主线程监听侦听 socket 上的可读事件，也就是监测是否有新连接来了。主线程和每个工作线程上都存在一个 epollfd。如果新连接来了，则在主线程的 handle_io_events() 中接收新连接。产生的新连接的 socket 句柄挂接到哪个线程的 epollfd 上呢？这里采取的做法是 round-robin 算法，即存在一个对象 CWorkerThreadManager 记录了各个工作线程上工作状态。伪码大致如下：

```
void attach_new_fd(int newsocketfd)
{
    workerthread = get_next_worker_thread(next);
    workerthread.attach_to_epollfd(newsocketfd);
    ++next;
    if (next > max_worker_thread_num)
    {
       next = 0;
    }
}
```

即先从第一个工作线程的 epollfd 开始挂接新来 socket，接着累加索引，这样下次就是第二个工作线程了。如果所以超出工作线程数目，则从第一个工作重新开始。这里解决了新连接 socket “负载均衡”的问题。在实际代码中还有个需要注意的细节就是：epoll_wait 的函数中的 struct epoll_event 数量开始到底要设置多少个才合理？存在的顾虑是，多了浪费，少了不够用，我在曾经一个项目中直接用的是 4096：
```
const int EPOLL_MAX_EVENTS = 4096;
const int dwSelectTimeout = 10000;
struct epoll_event events[EPOLL_MAX_EVENTS];
int nfds = epoll_wait(m_fdEpoll, events, EPOLL_MAX_EVENTS, dwSelectTimeout / 1000);
```

我在陈硕的 muduo 网络库中发现作者才用了一个比较好的思路，即动态扩张数量：开始是 n 个，当发现有事件的 fd 数量已经到达n个后，将 struct epoll_event 数量调整成 2n 个，下次如果还不够，则变成 4n 个，以此类推，作者巧妙地利用 stl::vector 在内存中的连续性来实现了这种思路：
```
//初始化代码
std::vector<struct epoll_event> events_(16);

//线程循环里面的代码
while (m_bExit)
{
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), 1);
    if (numEvents > 0)
    {
        if (static_cast<size_t>(numEvents) == events_.size())
        {
          	events_.resize(events_.size() * 2);
        }
    }
}
```

读到这里，你可能觉得工作线程所做的工作也不过就是调用 handle_io_events() 来接收网络数据，其实不然，工作线程也可以做程序业务逻辑上的一些工作。也就是在 handle_other_things() 里面。那如何将这些工作加到 handle_other_things() 中去做呢？写一个队列，任务先放入队列，再让 handle_other_things() 从队列中取出来做？我在该项目中也借鉴了 muduo 库的做法。即 handle_other_things() 中调用一系列函数指针，伪码如下：

	void do_other_things()
	{
		  somefunc();
	}
	
	//m_functors是一个stl::vector,其中每一个元素为一个函数指针
	void somefunc()
	{
	    for (size_t i = 0; i < m_functors.size(); ++i)
	    {
	        m_functors[i]();
	    }
	    m_functors.clear();
	}
当任务产生时，只要我们将执行任务的函数 push_back 到 m_functors 这个 stl::vector 对象中即可。但是问题来了，如果是其他线程产生的任务，两个线程同时操作 m_functors，必然要加锁，这也会影响效率。muduo 是这样做的：

	void add_task(const Functor& cb)
	{
	    std::unique_lock<std::mutex> lock(mutex_);
	    m_functors.push_back(cb);	
	}
	
	void do_task()
	{
	    std::vector<Functor> functors;
	    {
	        std::unique_lock<std::mutex> lock(mutex_);
	        functors.swap(m_functors);
	    }
	    for (size_t i = 0; i < functors.size(); ++i)
	    {
	        functors[i]();
	    }
	}

看到没有，利用一个栈变量 functors 将 m_functors 中的任务函数指针倒换（swap）过来了，这样大大减小了对 m_functors 操作时的加锁粒度。前后变化：变化前，相当于原来A给B多少东西，B消耗多少，A给的时候，B不能消耗；B消耗的时候A不能给。现在变成A将东西放到篮子里面去，B从篮子里面拿，B如果拿去一部分后，只有消耗完了才会来拿，或者A通知B去篮子里面拿，而B忙碌时，A是不会通知B来拿，这个时候A只管将东西放在篮子里面就可以了。

	bool bBusy = false;
	void add_task(const Functor& cb)
	{
	    std::unique_lock<std::mutex> lock(mutex_);
	    m_functors_.push_back(cb);
	    //B不忙碌时只管往篮子里面加，不要通知B
	    if (!bBusy)
	    {
	     	   wakeup_to_do_task();
	    }
	}
	
	void do_task()
	{
	    bBusy = true;
	    std::vector<Functor> functors;
	    {
	        std::unique_lock<std::mutex> lock(mutex_);
	        functors.swap(pendingFunctors_);
	    }
	    for (size_t i = 0; i < functors.size(); ++i)
	    {
	        functors[i]();
	    }
	
	    bBusy = false;
	}
看，多巧妙的做法！

因为每个工作线程都存在一个 m_functors，现在问题来了，如何将产生的任务均衡地分配给每个工作线程。这个做法类似上文中如何将新连接的 socket 句柄挂载到工作线程的 epollfd 上，也是 round-robin 算法。上文已经描述，此处不再赘述。

还有种情况，就是希望任务产生时，工作线程能够立马执行这些任务，而不是等 epoll_wait 超时返回之后。这个时候的做法，就是使用一些技巧唤醒 epoll_wait，linux 系统可以使用 socketpair 或 timerevent、eventfd 等技巧（这个细节在我的博文《服务器端编程心得（一）—— 主线程与工作线程的分工》已经详细介绍过了）。

上文中留下三个问题：

1. 数据库线程任务队列A中的任务从何而来，目前只有消费者，没有生产者，那么生产者是谁？
1. 数据库线程任务队列B中的任务将去何方，目前只有生产者没有消费者。
1. 业务层的数据如何发送出去？

问题1的答案是：业务层产生任务可能会交给数据库任务队列A，这里的业务层代码可能就是工作线程中 do_other_things() 函数执行体中的调用。至于交给这个9个数据库线程的哪一个的任务队列，同样采用了round-robin 算法。所以就存在一个对象 CDbThreadManager 来管理这九个数据库线程。下面的伪码是向数据库工作线程中加入任务：

    bool CDbThreadManager::AddTask(IMysqlTask* poTask )
    {
        if (m_index >= m_dwThreadsCount)
        {
            m_index = 0;
        }
        return m_aoMysqlThreads[m_index++].AddTask(poTask);
    }

同理问题2中的消费者也可能就是 do_other_things() 函数执行体中的调用。

现在来说问题3，业务层的数据产生后，经过 TcpSession 装包后，需要发送的话，产生任务丢给工作线程的 do_other_things()，然后在相关的 Channel 里面发送，因为没有监测该 socket 上的可写事件，所以该数据可能调用 send() 或者 write() 时会阻塞，没关系，sleep()一会儿，继续发送，一直尝试，到数据发出去。伪码如下：

	bool Channel::Send()
	{
	    int offset = 0;
	    while (true)
	    {
	        int n = ::send(socketfd, buf + offset, length - offset);
	        if (n == -1)
	        {
	            if (errno == EWOULDBLOCK)
	            {
	                ::sleep(100);
	                continue;
	            }
	        }
	        //对方关闭了socket，这端建议也关闭
	        else if (n == 0)
	        {
	            close(socketfd);
	            return false;
	        }
	        offset += n;
	        if (offset >= length)
	        {
	            break;
	        }
	    }
	
	    return true;	
	}



最后，还有一个日志线程没有介绍，高性能的日志实现方案目前并不常见。限于文章篇幅，下次再介绍。
