#include <stdexcept>
#include <iostream>
#include <cstring>

#include "utils.hpp"
#include "data_pack.hpp"

DataPack::DataPack() {}
DataPack::~DataPack () {}

//GetHeadLen 获取头长度方法
uint32_t DataPack::getHeadLen()  {
    return 8;
}

//Pack 封包:len,ID,data
void DataPack::pack(Message msg, char* dataBuf) {
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
    std::memcpy(dataBuf + startPos, msg.getData().data(), msg.getData().size() + 1);
}

//Unpack 拆包:读取数据包头
Message DataPack::unpack(const char* dataBuf) {
    Message msg;
    //读dataLen
    msg.setMsgLen((uint32_t)dataBuf[0]);
    //读msgID
    msg.setMsgID((uint32_t)dataBuf[4]);

    if(GlobalObject::getInstance().MaxPackageSize > 0
            && msg.getMsgLen() > GlobalObject::getInstance().MaxPackageSize) {
        throw std::logic_error("excess MaxPackageSize");
    }
    return msg;
}

//Pack 封包:len,ID,data
void DataPack::pack(Message msg, boost::asio::streambuf& dataBuf) {
    std::iostream ios(&dataBuf);
    //dataLen写进buf
    uint32_t len = msg.getMsgLen();
    ios.write((char*)(&len), 4);
    //dataID写进buf
    uint32_t id = msg.getMsgID();
    ios.write((char*)(&id), 4);
    //data写进buf
    ios.write(msg.getData().data(), msg.getData().size());
}

//Unpack 拆包:读取数据包头
Message DataPack::unpack(boost::asio::streambuf& dataBuf) {
    std::iostream ios(&dataBuf);
    Message msg;
    uint32_t len = 0;
    uint32_t id = 0;
    //读dataLen
    ios.read((char*)(&len), 4);
    msg.setMsgLen(len);
    //读msgID
    ios.read((char*)(&id), 4);
    msg.setMsgID(id);

    if(GlobalObject::getInstance().MaxPackageSize > 0
            && len > GlobalObject::getInstance().MaxPackageSize) {
        throw std::logic_error("excess MaxPackageSize");
    }
    return msg;
}
