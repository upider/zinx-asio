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
										  
## 协程使用

+ 第一版:在启动每个Connection的read和write时，将其包装成协程且序列化读写操作
读操作结束,将计算任务提交到TaskWorkerPool(线程池),并将协程挂起,计算结束恢复协程进行写操作
+ 第二版:将第一版的read和write的一个协程拆成两个,一个连接不会阻塞其他连接,将计算任务提交到TaskWorkerPool(线程池),同时提交异步写任务到IO线程
