/**
 * @file forever_client.cpp
 * @brief 不停地发送消息直到超时
 * @author yzz
 * @version 1.0
 * @date 2020-05-08
 */
#include <iostream>
#include <thread>
#include <vector>
#include <stdexcept>

#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>

#include "data_pack.hpp"
#include "message.hpp"

using namespace boost::asio;

//模拟客户端
void client0(boost::asio::io_context& ioc) {
    printf("=============client start==============\n");
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::endpoint endpoint(
        ip::address::from_string("127.0.0.1"), 9999);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    char m[34] = "hello this is client-0 message-0";
    boost::asio::streambuf buf;
    size_t size = zinx_asio::DataPack().getHeadLen();
    try {
        for (;;) {
            //消息打包
            zinx_asio::Message msgA(0, m, 29);
            //使用string作为消息载体
            std::string charBuf;
            zinx_asio::DataPack().pack(charBuf, msgA);
            boost::asio::write(socket, boost::asio::buffer(charBuf));

            zinx_asio::Message msgB(1, "hello this is client-0 message-1", 33);
            zinx_asio::DataPack().pack(buf, msgB);
            boost::asio::write(socket, buf.data());
            //buf.consume(buf.size());
            //消息拆包
            boost::asio::read(socket, buf, transfer_exactly(size));
            auto msg2 = zinx_asio::DataPack().unpack(buf);
            //buf.consume(size);
            boost::asio::read(socket, buf, transfer_exactly(msg2.getMsgLen()));
            std::cout <<  "Server send back " << msg2.getMsgLen() << " bytes"
                      << "message is " << &buf << std::endl;
            //buf.consume(buf.size());
        }
        socket.shutdown(boost::asio::socket_base::shutdown_send);
        socket.close();
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        socket.close();
    }
}

//模拟客户端
void client1(boost::asio::io_context& ioc) {
    printf("=============client start==============\n");
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::endpoint endpoint(
        ip::address::from_string("127.0.0.1"), 8888);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    char m[34] = "hello this is client-1 message-0";
    boost::asio::streambuf buf;
    size_t size = zinx_asio::DataPack().getHeadLen();
    try {
        for (;;) {
            //消息打包
            zinx_asio::Message msgA(0, m, 29);
            //使用string作为消息载体
            std::string charBuf;
            zinx_asio::DataPack().pack(charBuf, msgA);
            boost::asio::write(socket, boost::asio::buffer(charBuf));

            zinx_asio::Message msgB(1, "hello this is client-1 message-1", 33);
            zinx_asio::DataPack().pack(buf, msgB);
            boost::asio::write(socket, buf.data());
            buf.consume(buf.size());
            //消息拆包
            buf.prepare(size);
            boost::asio::read(socket, buf, transfer_exactly(size));
            auto msg2 = zinx_asio::DataPack().unpack(buf);
            buf.consume(size);
            boost::asio::read(socket, buf, transfer_exactly(msg2.getMsgLen()));
            std::cout <<  "Server send back " << msg2.getMsgLen() << " bytes"
                      << "message is " << &buf << std::endl;
            buf.consume(buf.size());
        }
        socket.shutdown(boost::asio::socket_base::shutdown_send);
        socket.close();
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        socket.close();
    }
}

int main(int argc, char *argv[]) {
    std::vector<boost::asio::io_context*> v;
    for (int i = 0; i < 2; ++i) {
        v.push_back(new boost::asio::io_context(2));
    }

    std::thread t2([&v]() {
        client1(*v[1]);
        std::cout << "No." << 1 << " connection" << std::endl;
        v[1]->run();
    });

    client0(*v[0]);
    std::cout << "No." << 0 << " connection" << std::endl;
    v[0]->run();
    t2.join();

    //sleep(12);
    return 0;
}
