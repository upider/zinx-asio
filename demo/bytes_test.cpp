/**
 * @file bytes_test.cpp
 * @brief 测试ByteBuffer新增功能
 * @author yuzz
 * @version 1.0
 * @date 2020-05-09
 */

#include <iostream>
#include <algorithm> // for copy 
#include <iterator> // for back_inserter
#include <sstream>
#include <ext/pool_allocator.h>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

#include "data_pack.hpp"
#include "byte_buffer.hpp"

int main(void)
{
    //ByteBuffer<>的模板参数是分配器
    //这样就可以使用__gnu_cxx::__pool_alloc<char>
    //默认分配器std::::allocator<char>
    zinx_asio::ByteBuffer<__gnu_cxx::__pool_alloc<char>> buffer;
    //测试read-write
    std::cout << "==========read-write=========" << std::endl;
    buffer.writeInt8(90).writeUint16(50);
    std::cout << "after write buffer size = " << buffer.size() << std::endl;
    int8_t x = 0;
    uint16_t xx = 0;
    buffer.readInt8(x).readUint16(xx);
    std::cout << "after read buffer size = " << buffer.size() << std::endl;
    std::cout << (int8_t)x << std::endl;
    std::cout << xx << std::endl;
    std::cout << "buffer = " << buffer << '\n';

    //TODO:
    //测试数字的<<和>>

    //测试ByteBuffer和string之间的<<和>>
    std::cout << "===========和string的<<和>>========" << std::endl;
    buffer << "buffer1     buffer1";
    std::cout << "buffer = " << buffer << '\n';
    buffer << "buffer2";
    std::string str1;
    buffer >> str1;
    std::cout << "str1 = " << str1 << std::endl;
    std::cout << "buffer = " << buffer << '\n';

    //测试ByteBuffer和ByteBuffer的<<和>>
    std::cout << "===========ByteBuffer和ByteBuffer的<<和>>========" << std::endl;
    zinx_asio::ByteBuffer<> buffer2;
    buffer2 << "buffer2";
    buffer << "buffer";
    buffer << buffer2;
    std::cout << buffer.toString() << std::endl;
    buffer2 << buffer;
    std::cout << "buffer2 = " << buffer2 << '\n';

    //测试复制
    std::cout << "===========移动语义========" << std::endl;
    buffer2 << "ooooo";
    auto lam = [](zinx_asio::ByteBuffer<>&& buf) {
        std::cout << buf << std::endl;
    };
    lam(std::move(buffer2));

    //测试toString
    std::cout << "===========toString()========" << std::endl;
    std::string str;
    buffer << "oooooooooppppppppppp";
    str = buffer.toString();
    std::cout << "str = " << str << std::endl;
    std::cout << "buffer = " << buffer << std::endl;

    //测试clear()
    std::cout << "===========clear()========" << std::endl;
    buffer << "XXXXXXXXXXXXXXXXXXXX";
    std::cout << "before clear buffer size = " << buffer.size() << '\n';
    buffer.clear();
    std::cout << "after clear buffer size = " << buffer.size() << '\n';
    std::cout << "buffer = " << buffer << std::endl;

    //测试ByteBuffer相互复制
    std::cout << "==========复制(=)=========" << std::endl;
    zinx_asio::ByteBuffer<> buffer3;
    buffer << "test operator =";
    buffer3 = buffer;
    std::cout << "buffer3 = " << buffer3 << std::endl;
    std::cout << "buffer = " << buffer << std::endl;


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

    std::string str3 {"this is bytes test client"};
    for (int i = 0; i < 10; ++i) {
        uint32_t len = str3.size();
        uint32_t id = 0;
        buffer.writeUint32(len).writeUint32(id) << str3;

        std::cout << "Client Send data" << std::endl;
        boost::asio::write(socket, buffer.buf(), boost::asio::transfer_all());
        len = zinx_asio::DataPack().getHeadLen();
        std::cout << "Client Read data" << std::endl;
        boost::asio::read(socket, buffer.buf(), boost::asio::transfer_exactly(len));
        buffer >> len >> id;
        boost::asio::read(socket, buffer.buf(), boost::asio::transfer_exactly(len));
        std::cout <<  "Server send back " << len << " bytes"
                  << " MsgID = " << id
                  << " message is " << buffer << '\n';
        std::cout << "buffer size = " << buffer.size() << '\n';
    }

    socket.shutdown(boost::asio::socket_base::shutdown_send);
    socket.close();

    return 0;
}
