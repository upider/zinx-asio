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

//模拟客户端
void client0(boost::asio::io_context& ioc) {
    printf("=============client start==============\n");
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 9999);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    std::string str = "hello this is client-0 message-0";
    std::string str2 = "hello this is forever_client message";

    size_t size = zinx_asio::DataPack().getHeadLen();
    zinx_asio::ByteBuffer<> buffer;
    try {
        for (;;) {
            //消息打包
            buffer.writeUint32(str.size()).writeUint32(0);
            buffer << str;
            boost::asio::write(socket, buffer.buf(), boost::asio::transfer_exactly(buffer.size()));

            buffer.writeUint32(str2.size()).writeUint32(0);
            buffer << str2;
            boost::asio::write(socket, buffer.buf(), boost::asio::transfer_exactly(buffer.size()));
            //消息拆包
            zinx_asio::Message msg2;
            boost::asio::read(socket, msg2.getData().buf(), boost::asio::transfer_exactly(size));
            uint32_t len;
            uint32_t id;
            msg2.getData() >> len;
            msg2.getData() >> id;
            msg2.setMsgID(id);
            msg2.setMsgLen(len);

            boost::asio::read(socket, msg2.getData().buf(), boost::asio::transfer_exactly(len));
            std::cout <<  "Server send back " << msg2.getMsgLen() << " bytes"
                      << "message is " << msg2.getData() << std::endl;
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
        boost::asio::ip::address::from_string("127.0.0.1"), 8888);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    //使用一个ByteBuffer接收和发送
    zinx_asio::ByteBuffer<> buffer;
    //消息格式uint32-长度|uint32-ID|内容
    int i = 0;
    try {
        for (;;) {
            //写入len和id
            std::string s {"this is forever client"};
            buffer.writeUint32(s.size()).writeUint32(0);
            buffer << s;

            std::cout << "Send No." << i << " Message" << std::endl;
            std::cout << "buffer size before send = " << buffer.size() << std::endl;
            boost::asio::write(socket, buffer.buf(), boost::asio::transfer_all());

            std::cout << "Read No." << i << " Message" << std::endl;
            //uint32_t size = zinx_asio::DataPack().getHeadLen();
            uint32_t size = zinx_asio::DataPack().getHeadLen();
            char chars[size];
            std::cout << "Read Data Head" << std::endl;
            boost::asio::read(socket, boost::asio::buffer(chars, size), boost::asio::transfer_exactly(size));
            uint32_t id;
            size = chars[0];
            id = chars[4];
            std::cout << "Read Data Body" << std::endl;
            boost::asio::read(socket, buffer.buf(), boost::asio::transfer_exactly(size));
            std::cout <<  "Server send back " << size << " bytes"
                      << " MsgID = " << id
                      << " message is " << buffer << '\n';

            i++;
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

    //std::thread t2([&v]() {
    //    client1(*v[0]);
    //    std::cout << "No." << o << " connection" << std::endl;
    //    v[0]->run();
    //});

    client1(*v[1]);
    std::cout << "No." << 1 << " connection" << std::endl;
    v[1]->run();
    //t2.join();

    //sleep(12);
    return 0;
}
