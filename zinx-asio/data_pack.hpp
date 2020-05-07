#ifndef DATA_PACK_HPP
#define DATA_PACK_HPP

#include <inttypes.h>
#include <boost/asio/streambuf.hpp>
#include "message.hpp"

namespace zinx_asio {//namespace zinx_asio
class DataPack {
    public:
        static DataPack& getInstance() {
            //返回local static的引用
            static DataPack instance;
            return instance;
        }
        //Pack 封包:len,ID,data
        void pack(Message msg, char* dataBuf);
        void pack(Message msg, boost::asio::streambuf& dataBuf);
        Message unpack(const char* dataBuf);
        Message unpack(boost::asio::streambuf& dataBuf);
        uint32_t getHeadLen();
    private:
        DataPack();
        virtual ~DataPack ();
    public:
        DataPack(const DataPack&) = delete;
        DataPack& operator=(const DataPack&) = delete;
};

}//namespace zinx_asio
#endif /* DATA_PACK_HPP */
