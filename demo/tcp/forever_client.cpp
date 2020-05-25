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

#include "byte_buffer.hpp"
#include "data_pack.hpp"

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

    std::string str = "hello this is client0 message";

    size_t size = zinx_asio::DataPack().getHeadLen();
    zinx_asio::ByteBuffer<> buffer;
    try {
        for (;;) {
            buffer.clear();
            //消息打包
            uint32_t id = 0;
            uint32_t len = str.size();
            //write方法写入的数字最好有明确的类型
            buffer.write(len).write(id).write(str);
            std::cout << "client0 send buffer size = " << buffer.size() << std::endl;
            boost::asio::write(socket, buffer.data(), boost::asio::transfer_all());
            buffer.clear();

            //消息拆包
            zinx_asio::ByteBuffer<> buf;
            boost::asio::read(socket, buf.prepare(size), boost::asio::transfer_exactly(size));
            buf.commit(size);
            buf.read(len).read(id);

            boost::asio::read(socket, buf.prepare(len),
                              boost::asio::transfer_exactly(len));
            buf.commit(len);
            std::cout <<  "Server send back " << len << " bytes"
                      << " message is " << "\"" << buf.toString() << "\""
                      <<  '\n';
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

    //使用一个ByteBuffeStream接收和发送
    zinx_asio::ByteBuffer<> buffer;
    //消息格式uint32-长度|uint32-ID|内容
    int i = 0;
    try {
        for (;;) {
            buffer.clear();
            //写入len和id
            uint32_t id = 1;
            std::string s {"hello this is client1 message"};
            u_int32_t len = s.size();
            buffer.write(len).write(id).write(s);

            std::cout << "Send No." << i << " Message" << std::endl;
            std::cout << "buffer size before send = " << buffer.size() << std::endl;
            boost::asio::write(socket, buffer.data(), boost::asio::transfer_all());
            buffer.clear();

            std::cout << "Read No." << i << " Message" << '\n';
            uint32_t size = zinx_asio::DataPack().getHeadLen();
            char chars[size];
            std::cout << "Read Data Head" << '\n';
            boost::asio::read(socket, boost::asio::buffer(chars, size), boost::asio::transfer_exactly(size));

            //解包
            auto head = zinx_asio::DataPack().unpack(chars);
            size = head.first;
            id = head.second;

            std::cout << "Read Data Body" << std::endl;
            boost::asio::read(socket, buffer.prepare(size), boost::asio::transfer_exactly(size));
            buffer.commit(size);
            std::cout <<  "Server send back " << size << " bytes"
                      << " MsgID = " << id
                      << " message is " << buffer.toString() << '\n';

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

    std::thread t2([&v]() {
        client0(*v[0]);
        std::cout << "No." << 0 << " connection" << std::endl;
        v[0]->run();
    });

    client1(*v[1]);
    std::cout << "No." << 1 << " connection" << std::endl;
    v[1]->run();

    t2.join();
    //sleep(12);
    return 0;
}
