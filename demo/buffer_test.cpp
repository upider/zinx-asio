#include <iostream>
#include <list>
#include <ext/pool_allocator.h>

#include "data_pack.hpp"
#include "byte_buffer_stream.hpp"
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

//模拟客户端
void client(boost::asio::io_context& ioc) {
    printf("=============client start==============\n");
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 8888);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    char* s2 = new char(20);
    char str[] = "This is s2";
    s2 = str;
    std::string s1 {"this is s1"};
    std::list<boost::asio::const_buffer> bufs;
    zinx_asio::ByteBufferStream<> buffer;
    bufs.push_back(boost::asio::buffer(s2, 10));
    bufs.push_back(boost::asio::buffer(s1.data(), s1.size()));
    try {
        for (uint32_t i = 0; i < 5; ++i) {
            //写入len和id
            uint32_t len = s1.size() + 10;
            uint32_t id = 0;
            buffer.writeUint32(len).writeUint32(id);

            std::cout << "Send Data Size = " << len << std::endl;
            boost::asio::write(socket, buffer.data(), boost::asio::transfer_exactly(buffer.size()));
            buffer.clear();
            boost::asio::write(socket, bufs);

            len = zinx_asio::DataPack().getHeadLen();
            std::cout << "Read Data Head: " << len << " bytes" << std::endl;
            boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
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


int main(void)
{
    boost::asio::io_context ioc;
    client(ioc);
    ioc.run();
    return 0;
}
