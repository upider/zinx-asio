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

#include "request.hpp"
#include "server.hpp"

//PingRouter 使用ping 测试路由
class PingRouter: virtual public zinx_asio::Router {
    public:
        PingRouter() {}
        virtual~PingRouter() {}
        //Handle ping
        void handle(zinx_asio::Request& request) {
            auto conn = request.getConnection();
            auto& data = request.getData();
            printf("Ping Handle------Receive from %d Connection, Message ID is %d, Message is \"%s\"\n",
                   conn->getConnID(), request.getMsgID(), data.data());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            conn->sendMsg(conn->getConnID(), "Server PingRouter get message", 30);
        }
        void preHandle(zinx_asio::Request&) {}
        void postHandle(zinx_asio::Request&) {}
};

//PingRouter 使用ping 测试路由
class HelloRouter: virtual public zinx_asio::Router {
    public:
        HelloRouter() {}
        virtual ~HelloRouter() {}
        //Handle ping
        void handle(zinx_asio::Request& request) {
            auto conn = request.getConnection();
            auto& data = request.getData();
            printf("Hello Handle------Receive from %d Connection, Message ID is %d, Message is \"%s\"\n",
                   conn->getConnID(), request.getMsgID(), data.data());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            conn->sendMsg(conn->getConnID(), "Server HelloRouter get message", 30);
        }
        void preHandle(zinx_asio::Request& request) {}
        void postHandle(zinx_asio::Request& request) {}
};


int main() {
    //创建server
    zinx_asio::Server s;
    //添加钩子函数
    s.setOnConnStart([](std::shared_ptr<zinx_asio::Connection>) {
        std::cout << "**************OnConnStart**************" << std::endl;
    });
    s.setOnConnStop([](std::shared_ptr<zinx_asio::Connection>) {
        std::cout << "**************OnConnStop***************" << std::endl;
    });
    //添加Router
    s.addRouter(0, std::make_shared<PingRouter>());
    s.addRouter(1, std::make_shared<HelloRouter>());

    //开始监听
    s.listen();
    s.getConnMgr()->addSocketOption(zinx_asio::ReuseAddr, true);

    //设置acceptor属性
    boost::asio::ip::tcp::acceptor::reuse_address option1(true);
    boost::asio::socket_base::keep_alive option2(true);
    s.setAcceptorOption(option1).setAcceptorOption(option2);

    //开始运行
    s.serve();
    return 0;
}
