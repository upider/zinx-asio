#include "udp_message.hpp"

namespace zinx_asio {//namespace zinx_asio

UDPMessage::UDPMessage() {}

UDPMessage::UDPMessage(std::size_t size): buffer_(size) {}

UDPMessage::~UDPMessage() {}

uint32_t UDPMessage::getMsgID()const {
    return -1;
}

void UDPMessage::setMsgID(uint32_t) {}

uint32_t UDPMessage::getMsgLen()const {
    return len_;
}

void UDPMessage::setMsgLen(uint32_t len) {
    len_ = len;
}

//设置消  息数组
void UDPMessage::setContent(const std::vector<char>& vec) {
    buffer_.write(vec);
}

void UDPMessage::setContent(const std::string& str) {
    buffer_.write(str);
}

void UDPMessage::setContent(const char* source, size_t size) {
    buffer_.write(source, size);
}

ByteBuffer<>& UDPMessage::bufferRef() {
    return buffer_;
}

std::string UDPMessage::contentToString() {
    return buffer_.toString();
}

std::list<boost::asio::const_buffer> UDPMessage::data() {
    std::list<boost::asio::const_buffer> list;
    list.push_back(buffer_.data());
    return list;
}

void UDPMessage::commit(std::size_t size) {
    buffer_.commit(size);
}

void UDPMessage::consume(std::size_t size) {
    buffer_.consume(size);
}

}//namespace zinx_asio
