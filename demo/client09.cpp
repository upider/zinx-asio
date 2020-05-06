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
    ip::tcp::endpoint endpoint(
        ip::address::from_string("127.0.0.1"), 9999);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    boost::asio::streambuf buf;
    try {
        size_t size = DataPack::getInstance().getHeadLen();
        for (uint32_t i = 0; i < 5; ++i) {
            //消息打包
            Message msgA(0, "hello this is client message", 29);
            DataPack::getInstance().pack(msgA, buf);
            boost::asio::write(socket, buf.data());
            buf.consume(buf.size());

			Message msgB(1, "hello this is client message", 29);
			DataPack::getInstance().pack(msgB, buf);
			boost::asio::write(socket, buf.data());
			buf.consume(buf.size());
			//消息拆包
            buf.prepare(size);
            boost::asio::read(socket, buf, transfer_exactly(size));
            auto msg2 = DataPack::getInstance().unpack(buf);
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
