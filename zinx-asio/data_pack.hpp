#ifndef DATA_PACK_HPP
#define DATA_PACK_HPP

#include <utility>
#include <inttypes.h>
#include <boost/asio/streambuf.hpp>

#include "byte_buffer_stream.hpp"
#include "message.hpp"

namespace zinx_asio {//namespace zinx_asio
class DataPack {
    public:
        DataPack();
        virtual ~DataPack ();

        ///pack方法：Message中的数据不变
        //Pack 封包:len,ID,data,message打包进dataBuf
        static void pack(char* dataBuf, Message& msg) ;
        //msg打包进vector,得到的vector是不可读的,但是可以直接用asio::buffer()发送
        static void pack(std::vector<char>& dataBuf, Message& msg) ;
        //msg打包进string,得到的string是不可读的,但是可以直接用asio::buffer()发送
        static void pack(std::string& dataBuf, Message& msg) ;
        //msg打包进streambuf,得到的string是不可读的,但是可以直接用asio::buffer()发送
        static void pack(boost::asio::streambuf& dataBuf, Message& msg) ;
        //msg打包进streambuf,得到的string是不可读的,但是可以直接用asio::buffer()发送
        template<typename T>
        static void pack(zinx_asio::ByteBufferStream<T>& dataBuf, Message& msg) ;
        //拆包:拿到msgLen和msgID
        //char* 中数据不变,返回(len,id)
        static std::pair<uint32_t, uint32_t> unpack(const char* dataBuf);
        //拆包:拿到msgLen和msgID
        //streambuf的前八个字节被取出,返回(len,id)
        static std::pair<uint32_t, uint32_t> unpack(boost::asio::streambuf& dataBuf);
        //拆包:拿到msgLen和msgID
        //Message的data中的前八个字节被取出,并拆包到message的len和id中,返回(len,id)
        static std::pair<uint32_t, uint32_t> unpack(Message&);

        //拆包:拿到msgLen和msgID
        //char* 中数据不变
        static void unpack(uint32_t &len, uint32_t &id, const char* dataBuf);
        //拆包:拿到msgLen和msgID
        //streambuf的前八个字节被取出
        static void unpack(uint32_t &len, uint32_t &id, boost::asio::streambuf& dataBuf);
        //拆包:拿到msgLen和msgID
        //Message的data中的前八个字节被取出,并拆包到message的len和id中
        static void unpack(uint32_t &len, uint32_t &id, Message&);
        static uint32_t getHeadLen();
    public:
        DataPack(const DataPack&) = delete;
        DataPack& operator=(const DataPack&) = delete;
};

}//namespace zinx_asio
#endif /* DATA_PACK_HPP */
