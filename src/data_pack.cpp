#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>

#include "utils.hpp"
#include "data_pack.hpp"

namespace zinx_asio {//namespace zinx_asio

DataPack::DataPack() {}
DataPack::~DataPack () {}

//GetHeadLen 获取头长度方法
uint32_t DataPack::getHeadLen()  {
    return 8;
}

//Pack 封包:len,ID,data,message放进dataBuf
//使用时注意dataBuf长度必须足够
void DataPack::pack(char* dataBuf, Message& msg) {
    int startPos = 0;
    //dataLen写进buf
    uint32_t len = msg.getMsgLen();
    std::memcpy(dataBuf + startPos, &len, sizeof(len));
    startPos += sizeof(len);
    //dataID写进buf
    uint32_t id = msg.getMsgID();
    std::memcpy(dataBuf + startPos, &id, sizeof(id));
    startPos += sizeof(id);
    //data写进buf
    msg.getData().read(dataBuf + startPos, len);
    //std::memcpy(dataBuf + startPos, msg.getData().data(), msg.getData().size());
}

//Pack 封包:len,ID,data
void DataPack::pack(boost::asio::streambuf& dataBuf, Message& msg) {
    std::ostream os(&dataBuf);
    //dataLen写进buf
    uint32_t len = msg.getMsgLen();
    os.write((char*)(&len), sizeof(len));
    //dataID写进buf
    uint32_t id = msg.getMsgID();
    os.write((char*)(&id), sizeof(id));
    //data写进buf
    os << msg.getData();
}

void DataPack::pack(std::string& dataBuf, Message& msg) {
    //dataLen写进buf
    uint32_t len = msg.getMsgLen();
    dataBuf.reserve(msg.getMsgLen() + getHeadLen());
    dataBuf[0] = (len >> 24) & 255;
    dataBuf[1] = (len >> 16) & 255;
    dataBuf[2] = (len >> 8) & 255;
    dataBuf[3] = len & 255;

    //dataID写进buf
    uint32_t id = msg.getMsgID();
    dataBuf[4] = (id >> 24) & 255;
    dataBuf[5] = (id >> 16) & 255;
    dataBuf[6] = (id >> 8) & 255;
    dataBuf[7] = id & 255;
    msg.getData() >> dataBuf;
}

void DataPack::pack(std::vector<char>& dataBuf, Message& msg) {
    //dataLen写进buf
    uint32_t len = msg.getMsgLen();
    dataBuf.reserve(msg.getMsgLen() + getHeadLen());
    dataBuf[0] = (len >> 24) & 255;
    dataBuf[1] = (len >> 16) & 255;
    dataBuf[2] = (len >> 8) & 255;
    dataBuf[3] = len & 255;

    //dataID写进buf
    uint32_t id = msg.getMsgID();
    dataBuf[4] = (id >> 24) & 255;
    dataBuf[5] = (id >> 16) & 255;
    dataBuf[6] = (id >> 8) & 255;
    dataBuf[7] = id & 255;
    for (size_t i = 0; i < msg.getMsgLen(); ++i) {
        dataBuf[i + 8] = msg.getData().buf().sbumpc();
    }
}

//Unpack 拆包:读取数据包头
std::tuple<uint32_t, uint32_t> DataPack::unpack(const char* dataBuf) {
    //读dataLen
    uint32_t len = (uint32_t)dataBuf[0];
    //读msgID
    uint32_t id = (uint32_t)dataBuf[4];

    if(GlobalObject::getInstance().MaxPackageSize > 0
            && len > GlobalObject::getInstance().MaxPackageSize) {
        throw std::logic_error("excess MaxPackageSize");
    }
    return std::make_tuple(len, id);
}

//Unpack 拆包:读取数据包头
std::tuple<uint32_t, uint32_t> DataPack::unpack(boost::asio::streambuf& dataBuf) {
    std::iostream ios(&dataBuf);
    uint32_t len = 0;
    uint32_t id = 0;
    //读dataLen
    ios.read((char*)(&len), 4);
    //读msgID
    ios.read((char*)(&id), 4);

    if(GlobalObject::getInstance().MaxPackageSize > 0
            && len > GlobalObject::getInstance().MaxPackageSize) {
        throw std::logic_error("excess MaxPackageSize");
    }
    return std::make_tuple(len, id);
}

}//namespace zinx_asio
