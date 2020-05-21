/**
 * @file idle_client.cpp
 * @brief 测试空闲的客户端连接
 * @author yuzz
 * @version 1.1
 * @date 2020-05-16
 */

#include <iostream>
#include <sstream>
#include <ext/pool_allocator.h>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

#include "data_pack.hpp"
#include "byte_buffer_stream.hpp"

int main(void)
{
    //ByteBuffeStream<>的模板参数是分配器
    //这样就可以使用__gnu_cxx::__pool_alloc<char>
    //默认分配器std::::allocator<char>
    zinx_asio::ByteBufferStream<__gnu_cxx::__pool_alloc<char>> buffer;

    //测试网络发送
    std::cout << "===========socket========" << std::endl;
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 8888);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    std::string str3 {"this is idle test client"};
    uint32_t len = str3.size();
    uint32_t id = 0;
    buffer.writeUint32(len).writeUint32(id) << str3;

    std::cout << "Client Send data" << std::endl;
    boost::asio::write(socket, buffer.data(), boost::asio::transfer_all());
    buffer.clear();

    len = zinx_asio::DataPack().getHeadLen();
    std::cout << "Client Read data" << std::endl;

    boost::asio::async_read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len),
    [&socket, &buffer, &len, &id](const boost::system::error_code & ec, std::size_t length) {
        if (ec) {
            std::cout << "Read Error: " << ec.message() << std::endl;
            socket.shutdown(boost::asio::socket_base::shutdown_send);
            socket.close();
            return -1;
        }
        buffer.commit(len);

        buffer >> len >> id;
        boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
        buffer.commit(len);

        std::cout <<  "Server send back " << len << " bytes"
                  << " MsgID = " << id
                  << " message is " << buffer << '\n';
    });

    ioc.run();

    boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
    socket.shutdown(boost::asio::socket_base::shutdown_send);
    socket.close();

    return 0;
}
