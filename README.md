# zinx-asio

## 致谢

1. 感谢zinx作者刘丹冰提供的Golang版本zinx视频教程
2. 感谢Netty项目提供的Multi-Reactor架构思路

## 基本情况
+ 基于Boost.Asio实现zinx框架
+ 依赖:Boost.Asio和nlohmann/json
+ 总体架构属于Multi-Reactor,每个io_context当做Reactor
+ 第一版：io_context per CPU
+ 第二版：
	1. IO服务使用io_context per CPU,计算服务使用io_context和boost::thread_group构成的线程池,可以设置io_context(配置文件的任务队列数)和thread数目 
    2. 对于IO线程池，io_context per CPU的设计很多文章说可以让效率最高，所以目前IO线程池是io_context per CPU的设计
	3. 将第一版的read和write的一个协程拆成两个,一个连接不会阻塞其他连接

## 组成模块                    
+ Server：服务器主体:
	+ 消息管理模块MessageManager:
		+ 消息路由模块Router:抽象基类，负责对消息进行处理
	+ 连接管理模块ConnManager
		+ 连接模块Connection:连接，抽象出每个客户端与服务器的连接

+ Message消息模块:负责承载消息
+ Request请求模块:负责将消息和连接封装在一起

## 运行过程
客户端请求建立连接---->服务器接受连接---->客户端发送消息---->消息包装成Message---->
Message包装成Request---->Connection对应的的消息管理模块调用Router---->Message发送回客户端

## 配置文件例子

```json
{
	"Name":"ZinxServerApp",      #服务器名称
	"ZinxVersion":"V0.9",        #服务器版本
	"MaxConn":300,               #最大连接数
	"MaxPackageSize":512,        #最大包（字节）
	"IOWorkerPoolSize":1,        #IO工作池
	"TaskWorkerPoolSize":2,      #业务工作池
	"TaskWorkerQueueNum":1,      #业务工作池队列数目
	"MaxConnTime":10,            #最大连接时间-秒

	"EndPoints":[                #监听的所有地址
		{
			"Host":"127.0.0.1",
			"TCPPort":9999
		},
		{
			"Host":"127.0.0.1",
			"TCPPort":8888
		}]
}
```

## 代码例子

1. 服务端代码:
请看demo,很详细,注意
`[Writer exits error] Bad file descriptor`和`Connection 0 Timer Error: Operation canceled`
是打印的log信息不是错误 
2. 客户端代码如下:
Message形式为uint32_t(len)|uint32_t(ID)|char(内容)
len表示内容部分长度

```c
    size_t size = zinx_asio::DataPack().getHeadLen();
	//建立socket
	boost::asio::io_context ioc;
	boost::asio::ip::tcp::socket socket(ioc);
	//建立地址
    boost::asio::ip::tcp::endpoint endpoint(
        ip::address::from_string("127.0.0.1"), 9999);

    try {
		//建立连接
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }
	
	//构造Message
	//Message的setData方法可以使用vector和char*设置发送内容
	//以后会提供更丰富的方法
	zinx_asio::Message msgB(1, "hello this is client message", 29);
	//消息打包
	zinx_asio::DataPack().pack(buf, msgB);
	//发送消息
	boost::asio::write(socket, buf.data());
	//消耗掉发送的消息
	buf.consume(buf.size());

	//读消息
    boost::asio::read(socket, buf, transfer_exactly(size));
	//消息拆包:读取消息头部
    auto msg2 = zinx_asio::DataPack().unpack(buf);
	//消耗掉使用的消息
    buf.consume(size);
	//读取消息体
    boost::asio::read(socket, buf, transfer_exactly(msg2.getMsgLen()));
	//打印
    std::cout <<  "Server send back " << msg2.getMsgLen() << " bytes"
              << "message is " << &buf << std::endl;
    buf.consume(buf.size());
```

## 协程使用

+ 第一版:在启动每个Connection的read和write时，将其包装成协程且序列化读写操作
读操作结束,将计算任务提交到TaskWorkerPool(线程池),并将协程挂起,计算结束恢复协程进行写操作
+ 第二版:将第一版的read和write的一个协程拆成两个,一个连接不会阻塞其他连接,将计算任务提交到TaskWorkerPool(线程池),同时提交异步写任务到IO线程

## TODO

1. ByteBuffer整合到服务器
