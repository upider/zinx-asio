/**
 * @file bytes_test.cpp
 * @brief 测试ByteBuffer新增功能
 * @author yuzz
 * @version 1.0
 * @date 2020-05-09
 */

#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

#include "data_pack.hpp"
#include "byte_buffer.hpp"

int main(void)
{
    //ByteBuffer<>的模板参数是分配器
    //这样就可以使用__gnu_cxx::__pool_alloc<char>
    //默认分配器std::::allocator<char>
    ByteBuffer<> buffer;
    //测试read-write
    std::cout << "==========read-write=========" << std::endl;
    buffer.writeInt8(50);
    buffer.writeUint16(50);
    int8_t x = 0;
    uint16_t xx = 0;
    buffer.readInt8(x);
    buffer.readUint16(xx);
    std::cout << x << std::endl;
    std::cout << xx << std::endl;

    //测试<<和>>
    std::cout << "===========<<和>>========" << std::endl;
    buffer.writeInt8(50);
    buffer.writeUint32(50);
    int8_t y = 0;
    uint32_t yy = 0;
    buffer >> y >> yy;
    std::cout << y << std::endl;
    std::cout << yy << std::endl;

    //测试网络发送
    std::cout << "===========socket========" << std::endl;
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::socket socket(ioc);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 9999);

    try {
        socket.connect(endpoint);
    } catch(boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    for (int i = 0; i < 10; ++i) {
        buffer.writeUint32(10).writeUint32(0);
        buffer << "0123456789";

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
                  << " message is " << buffer;
        std::cout << std::endl;
    }

    socket.cancel();
    socket.close();

    return 0;
}
