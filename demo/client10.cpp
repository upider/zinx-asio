/**
 * @file client10.cpp
 * @brief 开启多个线程作为客户端
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

#include "message.hpp"
#include "data_pack.hpp"
#include "byte_buffer.hpp"

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

    //消息格式uint32-长度|uint32-ID|内容
    zinx_asio::ByteBuffer<> buffer;
    try {
        for (uint32_t i = 0; i < 5; ++i) {
            //写入len和id
            std::string s {"this is client"};
            buffer.writeUint32(s.size()).writeUint32(0);
            buffer << s;

            //使用data()方法需要自己处理缓冲区指针
            //boost::asio::write(socket, buffer.data(), boost::asio::transfer_exactly(buffer.size()));
            //buffer.consume(buffer.size());

            boost::asio::write(socket, buffer.buf(), boost::asio::transfer_exactly(buffer.size()));
            uint32_t size = zinx_asio::DataPack().getHeadLen();
            boost::asio::read(socket, buffer.buf(), boost::asio::transfer_exactly(size));
            uint32_t id;
            buffer >> size >> id;
            boost::asio::read(socket, buffer.buf(), boost::asio::transfer_exactly(size));
            std::cout <<  "Server send back " << size << " bytes"
                      << " MsgID = " << id
                      << " message is " << buffer << std::endl;
            //std::cout << std::endl;
        }
        socket.shutdown(boost::asio::socket_base::shutdown_send);
        socket.close();
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        socket.close();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: client <connection num>" << std::endl;
        return 0;
    }
    std::vector<boost::asio::io_context*> v;
    for (int i = 0; i < atoi(argv[1]); ++i) {
        v.push_back(new boost::asio::io_context(2));
    }

    for (int i = 2; i <= atoi(argv[1]); ++i) {
        std::thread t([i, &v]() {
            client0(*v[i - 1]);
            std::cout << "No." << i << " connection" << std::endl;
            v[i - 1]->run();
        });
        t.join();
    }

    client0(*v[0]);
    v[0]->run();

    //sleep(12);
    return 0;
}
