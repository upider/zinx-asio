#include <iostream>
#include <ext/pool_allocator.h>

#include "byte_buffer_stream.hpp"

int main(void)
{
    //测试拷贝构造
    std::cout << "=============测试拷贝构造=============" << '\n';
    zinx_asio::ByteBufferStream<> buffer1;
    buffer1.write("buffer1", 7);
    zinx_asio::ByteBufferStream<__gnu_cxx::__pool_alloc<char>> buffer2(buffer1);
    std::cout << "buffer1 = " << buffer1 << '\n';
    std::cout << "buffer2 = " << buffer2 << '\n';

    //测试拷贝构造
    std::cout << "=============测试移动拷贝构造=============" << '\n';
    buffer1.write("buffer1", 7);
    zinx_asio::ByteBufferStream<__gnu_cxx::__pool_alloc<char>> buffer4(std::move(buffer1));
    std::cout << "buffer1 = " << buffer1 << '\n';
    std::cout << "buffer2 = " << buffer4 << '\n';

    //测试从vector输入和输出
    std::cout << "=============测试和vector交互=============" << '\n';
    std::vector<char> v;
    buffer1.write("buffer1", 7);
    std::cout << "buffer1 size = " << buffer1.size() << std::endl;
    std::cout << "+++++++++向vector输出+++++++++" << '\n';
    buffer1 >> v;
    std::cout << "vector = ";
    std::copy(v.begin(), v.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    std::cout << "+++++++++由vector输入+++++++++" << '\n';
    std::vector<char> v1{'g', 'l', 'o', 'b', 'a', 'l'};
    buffer1 << v1;
    std::cout << "vector = ";
    std::copy(v1.begin(), v1.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    std::cout << "+++++++++toVector+++++++++" << std::endl;
    buffer1 << "toVector";
    auto v2 = buffer1.toVector();
    std::cout << "vector = ";
    std::copy(v2.begin(), v2.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    std::cout << "+++++++++copyToVector+++++++++" << std::endl;
    buffer1 << "copyToVector";
    std::vector<char> v3;
    buffer1.copyToVector(v3);
    std::cout << "vector = ";
    std::copy(v3.begin(), v3.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    //测试从vector输入和输出
    std::cout << "=============测试和array交互=============" << '\n';
    std::cout << "+++++++++向array输出+++++++++" << std::endl;
    std::array<char, 14> arr{0};
    buffer1 << "to array";
    buffer1 >> arr;
    std::cout << "array = ";
    std::copy(arr.begin(), arr.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    std::cout << "+++++++++由array输入+++++++++" << std::endl;
    std::array<char, 14> arr1{'H', 'e', 'l', 'l', 'o'};
    buffer1 << arr1;
    std::cout << "array = ";
    std::copy(arr1.begin(), arr1.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    std::cout << "+++++++++copyToArray+++++++++" << std::endl;
    buffer1 << "copyToArray";
    std::array<char, 11> arr4;
    buffer1.copyToArray(arr4);
    std::copy(arr4.begin(), arr4.end(), std::ostream_iterator<char>(std::cout, " "));
    std::cout << '\n';
    std::cout << "buffer1 = " << buffer1 << '\n';

    //测试流式读入读出算数类型
    std::cout << "=============测试用<<和>>与算数类型交互=============" << '\n';
    buffer1.writeUint32(9);
    buffer1.writeUint8(9).writeInt32(90).writeDouble(100.00);
    uint32_t u32;
    uint8_t u8 = 0;
    int32_t i32 = 0;
    double db = 0.0;
    //测试流式读出算数类型
    std::cout << "+++++++++流式读出算数类型+++++++++" << std::endl;
    buffer1 >> u32 >> u8 >> i32 >> db;
    printf("uint32_t = %u, uint8_t = %u, int32_t = %d, double = %f\n", u32, u8, i32, db);
    std::cout << "buffer1 = " << buffer1 << '\n';
    //测试流式写入
    std::cout << "+++++++++流式写入算数类型+++++++++" << std::endl;
    buffer1 << u32 << u8 << i32 << db;
    printf("uint8_t = %u, int32_t = %d, double = %f\n", u8, i32, db);
    std::cout << "buffer1 = " << buffer1 << '\n';

    return 0;
}
