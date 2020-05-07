#include <iostream>
#include <cstring>
#include "message.hpp"

namespace zinx_asio {//namespace zinx_asio

Message::Message(uint32_t id, const char* buf, size_t size)
    : id_(id), msgLen_(size), data_(buf, buf + size) {
}

Message::Message() {}
Message::~Message() {}

uint32_t Message::getMsgLen()const {
    return msgLen_;
}

uint32_t Message::getMsgID()const {
    return id_;
}

void Message::setMsgLen(uint32_t l) {
    msgLen_ = l;
}

void Message::setMsgID(uint32_t i) {
    id_ = i;
}

std::vector<char>& Message::getData() {
    return data_;
}

//设置消息数组
void Message::setData(std::vector<char>& buf) {
    data_ = buf;
}
//设置消息数组
void Message::setData(const char* buf, size_t size) {
    std::vector<char> v(buf, buf + size);
    data_.swap(v);
    msgLen_ = data_.size();
}


Message& Message::operator=(const Message& other) {
    id_ = other.id_;
    msgLen_ = other.msgLen_;
    data_ = other.data_;
    return *this;
}

}//namespace zinx_asio
