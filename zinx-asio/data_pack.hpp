#ifndef DATA_PACK_HPP
#define DATA_PACK_HPP

#include <inttypes.h>
#include <boost/asio/streambuf.hpp>
#include "message.hpp"

namespace zinx_asio {//namespace zinx_asio
class DataPack {
    public:
        DataPack();
        virtual ~DataPack ();

        //Pack 封包:len,ID,data
        static void pack(char* dataBuf, const Message& msg) ;
        //msg打包进vector,得到的vector是不可读的,但是可以直接用asio::buffer()发送
        static void pack(std::vector<char>& dataBuf, const Message& msg) ;
        //msg打包进string,得到的string是不可读的,但是可以直接用asio::buffer()发送
        static void pack(std::string& dataBuf, const Message& msg) ;
        //msg打包进streambuf,得到的string是不可读的,但是可以直接用asio::buffer()发送
        static void pack(boost::asio::streambuf& dataBuf, const Message& msg) ;
        //拆包:拿到msgLen和msgID
        static Message unpack(const char* dataBuf);
        //拆包:拿到msgLen和msgID
        static Message unpack(boost::asio::streambuf& dataBuf);
        static uint32_t getHeadLen();
    public:
        DataPack(const DataPack&) = delete;
        DataPack& operator=(const DataPack&) = delete;
};

}//namespace zinx_asio
#endif /* DATA_PACK_HPP */
