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
#include "byte_buffer_stream.hpp"

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
    zinx_asio::ByteBufferStream<> buffer;
    std::string s {"This Is Client"};
    try {
        for (uint32_t i = 0; i < 5; ++i) {
            //写入len和id
            uint32_t len = s.size();
            uint32_t id = 0;
            buffer.writeUint32(len).writeUint32(id);
            buffer << s;

            std::cout << "Send Data Size = " << buffer.size() << std::endl;
            boost::asio::write(socket, buffer.data(), boost::asio::transfer_exactly(buffer.size()));
            buffer.clear();

            len = zinx_asio::DataPack().getHeadLen();
            std::cout << "Read Data Head: " << len << " bytes" << std::endl;
            boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
            buffer.commit(len);
            buffer >> len >> id;

            std::cout << "Read Data Body: " << len << " bytes" << std::endl;
            boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
            buffer.commit(len);
            std::cout <<  "Server send back " << len << " bytes"
                      << " MsgID = " << id
                      << " message is " << buffer << '\n';
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
