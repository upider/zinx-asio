/**
 * @file server10.cpp
 * @brief 服务器端例子
 * @author yzz
 * @version 1.0
 * @date 2020-05-08
 */
#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio.hpp>

#include "conn_manager.hpp"
#include "byte_buffer_stream.hpp"
#include "request.hpp"
#include "server.hpp"

//PingRouter 使用ping 测试路由
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
                //得到ByteBufferStream
                auto& data = request.getMsg()->getData();
                //使用toString会保留request中的消息内容
                printf("Ping Handle------Receive from %d Connection, Message ID is %d, ",
                       conn->getConnID(), request.getMsgID());
                std::cout <<  "Message is " << data.toString() << std::endl;
                //用自己声明ByteBufferStream回送消息
                byteBuf.writeUint32(5).writeUint32(999) << "Hello";
                conn->sendMsg(byteBuf);
                //conn->sendMsg(999, "Hello");
            }
        }
        void preHandle(zinx_asio::Request&) {}
        void postHandle(zinx_asio::Request&) {}

    private:
        zinx_asio::ByteBufferStream<> byteBuf;
};

//PingRouter 使用ping 测试路由
class HelloRouter: virtual public zinx_asio::Router {
    public:
        HelloRouter() {}
        virtual ~HelloRouter() {}
        //Handle ping
        void handle(zinx_asio::Request& request) {
            //msgID为1才处理
            if (request.getMsgID() == 1) {
                auto conn = request.getConnection();
                //得到ByteBufferStream
                auto& data = request.getMsg()->getData();
                printf("Hello Handle------Receive from %d Connection, Message ID is %d, ",
                       conn->getConnID(), request.getMsgID());
                std::cout <<  "Message is " << data << std::endl;
                //回送vector
                std::vector<char> v{'H', 'e', 'l', 'l', 'o', 'R', 'o', 'u', 't', 'e', 'r'};
                conn->sendMsg(1000, v);
            }
        }
        void preHandle(zinx_asio::Request& request) {}
        void postHandle(zinx_asio::Request& request) {}
};

int main() {
    //创建server
    zinx_asio::Server<boost::asio::ip::tcp> s(1, 1, 2);
    //设置空闲连接时间
    s.setMaxConnIdleTime(5);

    //监听两个地址
    s.addEndPoint("127.0.0.1", 9999);
    s.addEndPoint("127.0.0.1", 8888);

    //添加钩子函数,在连接开始时被调用
    s.setOnConnStart([](std::shared_ptr<zinx_asio::Connection>) {
        std::cout << "**************OnConnStart**************" << std::endl;
    });
    //添加钩子函数,在连接结束时被调用
    s.setOnConnStop([](std::shared_ptr<zinx_asio::Connection>) {
        std::cout << "**************OnConnStop***************" << std::endl;
    });

    //注册Router类名,注册后才能使用
    //ID,类名,这里使用的是宏定义,所以直接写类名
    RegisterRouterClass(0, PingRouter);
    RegisterRouterClass(1, HelloRouter);
    //添加Router,业务处理方法
    //ID,名称
    //添加的Router会依次调用(按照ID)
    s.addRouter(0, "PingRouter");
    s.addRouter(1, "HelloRouter");

    //开始监听
    s.listen();
    //给套接字设置套接字选项,要在listen之后
    s.getConnMgr()->addSocketOption(zinx_asio::ReuseAddr, true);

    //设置acceptor属性,也要在listen之后
    boost::asio::ip::tcp::acceptor::reuse_address option1(true);
    boost::asio::socket_base::keep_alive option2(true);
    s.setAcceptorOption(option1).setAcceptorOption(option2);

    //开始运行
    s.serve();
    return 0;
}
