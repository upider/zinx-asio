#include <iostream>
#include <ext/pool_allocator.h>

#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>

#include "data_pack.hpp"
#include "byte_buffer.hpp"

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

    //消息格式uint32-长度|uint32-ID|内容
    zinx_asio::ByteBuffer<> buffer;
    std::string s {"This Is Client"};
    try {
        for (uint32_t i = 0; i < 5; ++i) {
            buffer.clear();
            //写入len和id
            uint32_t len = s.size();
            uint32_t id = 0;
            buffer.write(len).write(id).write(s);

            std::cout << "Send Data Size = " << buffer.size() << std::endl;
            boost::asio::write(socket, buffer.data(), boost::asio::transfer_exactly(buffer.size()));
            buffer.clear();

            len = zinx_asio::DataPack().getHeadLen();
            std::cout << "Read Data Head: " << len << " bytes" << std::endl;
            boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
            buffer.commit(len);
            buffer.read(len).read(id);

            std::cout << "Read Data Body: " << len << " bytes" << std::endl;
            boost::asio::read(socket, buffer.prepare(len), boost::asio::transfer_exactly(len));
            buffer.commit(len);
            std::cout <<  "Server send back " << len << " bytes"
                      << " MsgID = " << id
                      << " message is " << buffer.toString() << '\n';
        }
        socket.shutdown(boost::asio::socket_base::shutdown_send);
        socket.close();
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        socket.close();
    }
}


int main(int argc, char *argv[])
{
    //测试read,write
    std::cout << "=======测试read,write========" << std::endl;
    zinx_asio::ByteBuffer<> buffer(64);
    uint32_t u32 = 999;
    buffer.write(&u32, sizeof(u32));
    std::cout << buffer.size() << std::endl;
    uint32_t x;
    buffer.read((char*)&x, 4);
    std::cout << "x = " << x << std::endl;
    std::cout << "buffer size = " << buffer.size() << std::endl;

    //测试拷贝构造
    std::cout << "=======测试拷贝构造========" << std::endl;
    zinx_asio::ByteBuffer<__gnu_cxx::__pool_alloc<char>> buffer2(buffer);

    //测试getRawBuffer
    std::cout << "=======测试getRawBuffer========" << std::endl;
    buffer.write(&u32, 4);
    buffer.write(&u32, sizeof(u32));
    buffer.write(&u32, sizeof(u32));
    buffer.write(&u32, sizeof(u32));
    buffer.write(&u32, sizeof(u32));
    buffer.write(&u32, sizeof(u32));
    buffer.write(&u32, sizeof(u32));
    uint32_t* u32Array = (uint32_t*)buffer.getRawBuffer().first;
    u32Array[0] += 1;
    std::copy(u32Array, u32Array + buffer.getRawBuffer().second / sizeof(uint32_t),
              std::ostream_iterator<uint32_t>(std::cout, " "));
    std::cout << '\n';

    //测试数字读写
    std::cout << "=======测试数字读写========" << std::endl;
    buffer.clear();
    int num1 = 99;
    int32_t num2 = -99;
    buffer.write(num1).write(num2);
    std::cout << buffer.size() << std::endl;
    num1 = 0;
    num2 = 0;
    buffer.read(num1).read(num2);
    std::cout << "num1 = " << num1 << std::endl;
    std::cout << "num2 = " << num2 << std::endl;
    std::cout << "buffer size = " << buffer.size() << std::endl;

    //toVector
    std::cout << "=========toVector=========" << std::endl;
    int64_t i64 = 777;
    buffer.write(&i64, sizeof(i64));
    i64++;
    buffer.write(&i64, sizeof(i64));
    i64++;
    buffer.write(&i64, sizeof(i64));
    i64++;
    buffer.write(&i64, sizeof(i64));
    std::vector<int64_t> v = buffer.toVector<int64_t>();
    std::cout << v.size() << std::endl;
    std::copy(v.begin(), v.end(),
              std::ostream_iterator<uint32_t>(std::cout, " "));
    std::cout << '\n';

    //write(string)
    buffer.clear();
    std::cout << "=========write(string)=========" << '\n';
    buffer.write("hello");

    //toString
    std::cout << "=========toString=========" << '\n';
    std::cout << buffer.toString() << std::endl;
    std::cout << "buffer size = " << buffer.size() << std::endl;

    //writeableSize
    std::cout << "=========writeableSize=========" << '\n';
    std::cout << "buffer writeableSize = " << buffer.writeableSize() << std::endl;

    //read(string)
    std::cout << "=========writea(string)=========" << '\n';
    std::string s;
    buffer.read(s);
    std::cout << "s = " << s << '\n';
    std::cout << buffer.size() << std::endl;

    //read(vector)-write(vector)
    std::cout << "========read(vector)-write(vector)==========" << '\n';
    std::vector<int> v2{3, 45, 90};
    buffer.write(v2);
    v2.clear();
    std::cout << "buffer size = " << buffer.size() << std::endl;
    buffer.read(v2);
    std::cout << "buffer size = " << buffer.size() << std::endl;
    std::copy(v2.begin(), v2.end(),
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';

    //read(array)-write(array)
    std::cout << "========read(array)-write(array)==========" << '\n';
    std::array<uint16_t, 5> arr{45, 66, 11, 109, 999};
    buffer.write(arr);
    std::cout << "buffer size = " << buffer.size() << std::endl;
    std::array<char, 9> arr2;
    buffer.read(arr2);
    std::copy(arr2.begin(), arr2.end(),
              std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer size = " << buffer.size() << std::endl;

    //getRawBuffer
    std::cout << "========getRawBuffer By Type==========" << '\n';
    buffer.clear();
    //放进6个uint16
    buffer.write(arr);
    //按照uint32拿出
    auto pair = buffer.getRawBuffer<uint32_t>();
    //检查大小是否正确
    std::cout << "raw buffer size = " << pair.second << std::endl;
    std::copy(pair.first, pair.first + pair.second,
              std::ostream_iterator<uint32_t>(std::cout, " "));
    std::cout << '\n';

    //测试网络传输
    std::cout << "========测试网络传输==========" << '\n';
    boost::asio::io_context ioc;
    client(ioc);
    ioc.run();

    return 0;
}
