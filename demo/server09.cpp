#include <iostream>
#include <thread>
#include <chrono>

#include "request.hpp"
#include "server.hpp"

//PingRouter 使用ping 测试路由
class PingRouter: virtual public Router {
    public:
        PingRouter() {}
        virtual~PingRouter() {}
        //Handle ping
        void handle(Request& request) {
            auto conn = request.getConnection();
            auto& data = request.getData();
            printf("Ping Handle------Receive from %d Connection, Message ID is %d, Message is \"%s\"\n",
                   conn->getConnID(), request.getMsgID(), data.data());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            conn->sendMsg(conn->getConnID(), "Server PingRouter get message", 30);
        }
        void preHandle(Request&) {}
        void postHandle(Request&) {}
};

//PingRouter 使用ping 测试路由
class HelloRouter: virtual public Router {
    public:
        HelloRouter() {}
        virtual ~HelloRouter() {}
        //Handle ping
        void handle(Request& request) {
            auto conn = request.getConnection();
            auto& data = request.getData();
            printf("Hello Handle------Receive from %d Connection, Message ID is %d, Message is \"%s\"\n",
                   conn->getConnID(), request.getMsgID(), data.data());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            conn->sendMsg(conn->getConnID(), "Server HelloRouter get message", 30);
        }
        void preHandle(Request& request) {}
        void postHandle(Request& request) {}
};


int main() {
    //创建server
    Server s;
    //添加钩子函数
    s.setOnConnStart([](std::shared_ptr<Connection>) {
        std::cout << "OnConnStart" << std::endl;
    });
    s.setOnConnStop([](std::shared_ptr<Connection>) {
        std::cout << "OnConnStop" << std::endl;
    });
    //添加Router
    s.addRouter(0, std::make_shared<PingRouter>());
    s.addRouter(1, std::make_shared<HelloRouter>());
    s.serve();
    return 0;
}
