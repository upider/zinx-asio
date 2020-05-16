# zinx-asio

## 致谢

1. 感谢zinx作者刘丹冰提供的Golang版本zinx视频教程
2. 感谢Netty项目提供的Multi-Reactor架构思路

## 基本情况
+ 基于Boost.Asio实现zinx框架
+ 依赖:Boost.Asio和nlohmann/json(git submodule)
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

## 协程使用

+ 第一版:在启动每个Connection的read和write时，将其包装成协程且序列化读写操作
读操作结束,将计算任务提交到TaskWorkerPool(线程池),并将协程挂起,计算结束恢复协程进行写操作
+ 第二版:将第一版的read和write的一个协程拆成两个,一个连接不会阻塞其他连接,将计算任务提交到TaskWorkerPool(线程池),同时提交异步写任务到IO线程

## 重要组件
1. ByteBuffer: 由boost::asio::basic_streambuf包装,具有流式API:
   所有流式API(>>和<<)以及readXXX和writeXXX,都会改变ByteBuffer内容
   ```c
	//测试从vector输入和输出
    std::cout << "=============测试和vector交互=============" << '\n';
    std::vector<char> v;
    buffer1 << "buffer1";
    std::cout << "+++++++++向vector输出+++++++++" << '\n';
	//buffer1内容被转移到v
    buffer1 >> v;
    std::cout << "vector = ";
    std::copy(v.begin(), v.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

	//copyToXXX不会改变ByteBuffer内容
	std::cout << "+++++++++copyToVector+++++++++" << std::endl;
    buffer1 << "copyToVector";
    std::vector<char> v3;
    buffer1.copyToVector(v3);
    std::cout << "vector = ";
    std::copy(v3.begin(), v3.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';
   ```
   你还可以使用C++的扩展分配器
   ```c
   #include <ext/pool_allocator.h>
   //拷贝构造
   zinx_asio::ByteBuffer<__gnu_cxx::__pool_alloc<char>> buffer2(buffer1);
   ```

2. Message: 包含消息ID,len和内容
   客户端与服务器的交互(TCP)是以消息的形式进行的,每条消息必须包含len,ID,content,
   格式:uint32_t|uint32_t|bytes,你可以将len,ID,content都放进ByteBuffer,然后发送
   ByteBuffer,也可以直接发送ID和content
   ```c
	//得到对应连接
    auto conn = request.getConnection();
    //得到ByteBuffer
    auto& data = request.getMsg()->getData();
    //使用toString会保留request中的消息内容
    printf("Ping Handle------Receive from %d Connection, Message ID is %d, ",
           conn->getConnID(), request.getMsgID());
    std::cout <<  "Message is " << data.toString() << std::endl;
    //用自己声明ByteBuffer回送消息
    byteBuf.writeInt32(5).writeInt32(999) << "Hello";
    conn->sendMsg(byteBuf);
   ```
   或者
   ```c
   //sendMsg有多个重载版本
   std::string str;
   conn->sendMsg(msgID,str);
   ```
3. Request: 包装了Connection和Message(shared_ptr的形式),在添加的Router之间传递,如
   果在某一个Router里数据被取出,后面的Router将不会拿到数据

4. Router：包装了对消息的处理方法,要给每个Router唯一的ID,并在使用前注册,可以重载
   三个方法,需要继承Router类(最好虚继承):
   ```c
   void preHandle(zinx_asio::Request&) {}
   void handle(zinx_asio::Request& request) {}
   void postHandle(zinx_asio::Request&) {}
   ```
   ```c
   class PingRouter: virtual public zinx_asio::Router {
    public:
        PingRouter() {}
        virtual ~PingRouter() {}
        //Handle ping
        void handle(zinx_asio::Request& request) {
            //msgID为0才处理
            if (request.getMsgID() == 0) {
                //得到对应连接
                auto conn = request.getConnection();
                //得到ByteBuffer
                auto& data = request.getMsg()->getData();
                //使用toString会保留request中的消息内容
                printf("Ping Handle------Receive from %d Connection, Message ID is %d, ",
                       conn->getConnID(), request.getMsgID());
                std::cout <<  "Message is " << data.toString() << std::endl;
                //用自己声明ByteBuffer回送消息
                byteBuf.writeInt32(5).writeInt32(999) << "Hello";
                conn->sendMsg(byteBuf);
            }
        }
        void preHandle(zinx_asio::Request&) {}
        void postHandle(zinx_asio::Request&) {}

    private:
        zinx_asio::ByteBuffer<> byteBuf;
   };
   ```

5. DataPack:数据打包和拆包的行为封装,让数据打包和拆包更方便

6. Server:服务器主体

## 注意
`[Writer exits error] Bad file descriptor`和
`Connection 0 Timer Error: Operation canceled`是打印的log信息不是错误

## TODO
1. 准备抽象出多个接口(纯虚类)
2. 支持UDP,HTTP(HTTP1.1,websocket)
3. 实现rpc功能(可能借助protobuf)
4. 日志功能
5. 支持SSL
6. 支持MySQL,Redis,postgreSQL,ORM
7. 命名服务(zookeeper)
