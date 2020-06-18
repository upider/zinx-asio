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

#include "data_pack.hpp"
#include "imessage.hpp"
#include "conn_manager.hpp"
#include "tcp_connection.hpp"
#include "connection_server.hpp"

//PingRouter 使用ping 测试路由
class PingRouter: virtual public zinx_asio::ConnectionRouter {
    public:
        PingRouter() {
            //用裸指针当做缓冲
            p = new char(50);
            zinx_asio::DataPack().pack(&len, p, 4);
            zinx_asio::DataPack().pack(&id, p + 4, 4);
            zinx_asio::DataPack().pack("Hello", p + 8, 5);
            //vector当做缓冲
            zinx_asio::DataPack().pack(&len, v, 4);
            zinx_asio::DataPack().pack(&id, v, 4);
            zinx_asio::DataPack().pack("Hello", v, 5);
        }
        virtual ~PingRouter() {
            delete []p;
        }
        //Handle ping
        void handle(std::shared_ptr<zinx_asio::IConnection> conn,
                    std::shared_ptr<zinx_asio::IMessage> msg) {
            //msgID为0才处理
            if (msg->getMsgID() == 0) {
                //得到ByteBuffer
                auto& data = msg->bufferRef();
                //使用toString会保留request中的消息内容
                printf("Ping Handle------Receive from %d Connection, Message ID is %d, ",
                       conn->getConnID(), msg->getMsgID());
                std::cout <<  "Message is " << data.toString() << std::endl;

                //char* 回送消息
                //conn->sendMsg(p, 13);
                //vector 回送消息
                //conn->sendMsg(v);

                //临时vector当做缓冲
                /*std::vector<char> tempv;*/
                //zinx_asio::DataPack().pack(&len, tempv, 4);
                //zinx_asio::DataPack().pack(&id, tempv, 4);
                //zinx_asio::DataPack().pack("Hello", tempv, 5);
                //conn->sendMsg(std::move(tempv));

                //临时streambuf 当做缓冲
                //boost::asio::streambuf buf;
                //std::ostream out(&buf);
                //out.write((char*)(&len), 4);
                //out.write((char*)(&id), 4);
                //out << "Hello";
                //conn->sendMsg(std::move(buf));

                //用自己声明ByteBuffer回送消息
                //byteBuf.write(5).write(999)write("Hello");
                //conn->sendMsg(byteBuf);

                //直接message回传
                //message可以直接作为数据被发送
                //加上新内容
                data.write(" this is server");
                msg->setMsgLen(msg->getMsgLen() + 15);
                conn->sendMsg(msg);
            }
        }
        void preHandle(std::shared_ptr<zinx_asio::IConnection> conn,
                       std::shared_ptr<zinx_asio::IMessage> msg) {}
        void postHandle(std::shared_ptr<zinx_asio::IConnection> conn,
                        std::shared_ptr<zinx_asio::IMessage> msg) {}

    private:
        //sendMsg不保证数据有效性,为保证异步操作时数据的有效性,有两种做法
        //1. 将数据放在Router类内部,Router不析构,数据就是有效的
        //2. 使用指针或智能指针
        //3. 使用std::move(),直接将数据转移
        //用裸指针当做缓冲
        char* p;
        //ByteBuffer当做缓冲
        zinx_asio::ByteBuffer<> byteBuf;
        //vector当做缓冲
        std::vector<char> v;
        uint32_t len = 5;
        uint32_t id = 999;
};

//PingRouter 使用ping 测试路由
class HelloRouter: virtual public zinx_asio::ConnectionRouter {
    public:
        HelloRouter() {}
        virtual ~HelloRouter() {}
        //Handle ping
        void handle(std::shared_ptr<zinx_asio::IConnection> conn,
                    std::shared_ptr<zinx_asio::IMessage> msg) {
            //msgID为1才处理
            if (msg->getMsgID() == 1) {
                //得到ByteBuffer
                auto& data = msg->bufferRef();
                printf("Hello Handle------Receive from %d Connection, Message ID is %d, ",
                       conn->getConnID(), msg->getMsgID());
                std::cout <<  "Message is " << data.toString() << std::endl;
                //回送vector
                //std::vector<char> v{'H', 'e', 'l', 'l', 'o', 'R', 'o', 'u', 't', 'e', 'r'};
                //conn->sendMsg(std::move(v));
            }
        }
        void preHandle(std::shared_ptr<zinx_asio::IConnection> conn,
                       std::shared_ptr<zinx_asio::IMessage> msg) {}
        void postHandle(std::shared_ptr<zinx_asio::IConnection> conn,
                        std::shared_ptr<zinx_asio::IMessage> msg) {}
};

int main() {
    //创建server
    zinx_asio::ConnectionServer<zinx_asio::TCPConnection> s(1, 1, 2, "ConnectionServer", "0.1");
    //设置空闲连接时间
    s.setMaxConnIdleTime(5);

    //监听两个地址
    s.addEndPoint("127.0.0.1", 9999);
    s.addEndPoint("127.0.0.1", 8888);

    //添加钩子函数,在连接开始时被调用
    s.setOnConnStart([](std::shared_ptr<zinx_asio::TCPConnection>) {
        std::cout << "**************OnConnStart**************" << std::endl;
    });
    //添加钩子函数,在连接结束时被调用
    s.setOnConnStop([](std::shared_ptr<zinx_asio::TCPConnection>) {
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
    boost::asio::socket_base::keep_alive option(true);
    s.getConnOptionMgr()->setOption(option);

    //设置acceptor属性,也要在listen之后
    boost::asio::ip::tcp::acceptor::reuse_address option1(true);
    boost::asio::socket_base::keep_alive option2(true);
    s.setAcceptorOption(option1).setAcceptorOption(option2);

    //开始运行
    s.serve();
    return 0;
}
