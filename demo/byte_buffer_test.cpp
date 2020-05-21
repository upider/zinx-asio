#include <iostream>
#include "byte_buffer.hpp"

int main(int argc, char *argv[])
{
    zinx_asio::ByteBuffer<> buffer(64);
    uint32_t u32 = 999;
    buffer.write(&u32, 4);
    std::cout << buffer.size() << std::endl;

    return 0;
}
