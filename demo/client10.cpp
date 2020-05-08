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

    char m[30] = "hello this is client message";
    boost::asio::streambuf buf;
    size_t size = zinx_asio::DataPack().getHeadLen();
    try {
        for (uint32_t i = 0; i < 5; ++i) {
            //消息打包
            zinx_asio::Message msgA(0, m, 29);
            //使用string作为消息载体
            std::string charBuf;
            zinx_asio::DataPack().pack(charBuf, msgA);
            boost::asio::write(socket, boost::asio::buffer(charBuf));

            zinx_asio::Message msgB(1, "hello this is client message", 29);
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
