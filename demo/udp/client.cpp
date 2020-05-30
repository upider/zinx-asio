#include <iostream>
#include <boost/asio.hpp>

#include "byte_buffer.hpp"
#include <ext/pool_allocator.h>

using namespace boost::asio;

int main(void)
{
    //ByteBuffeStream<>的模板参数是分配器
    //这样就可以使用__gnu_cxx::__pool_alloc<char>
    //默认分配器std::::allocator<char>
    zinx_asio::ByteBuffer<__gnu_cxx::__pool_alloc<char>> buffer;

    boost::asio::io_context ioc;
    boost::asio::ip::udp::socket socket(ioc);

    ip::udp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 8888);
    socket.open(endpoint.protocol());
    for (int i = 0; i < 10000; ++i) {
        buffer.write("this is client");
        socket.send_to(buffer.data(), endpoint);
        buffer.clear();
        auto len = socket.receive(buffer.prepare(128));
        buffer.commit(len);
        std::cout << "server send back "
                  << "\"" << buffer.toString() << "\"" << '\n';
        buffer.clear();
    }

    ioc.run();

    return 0;
}
