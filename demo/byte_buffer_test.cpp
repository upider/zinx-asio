#include <iostream>
#include <iterator>
#include "byte_buffer.hpp"

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

    return 0;
}
