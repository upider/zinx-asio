#include <iostream>
#include <cstring>
#include "message.hpp"

namespace zinx_asio {//namespace zinx_asio

Message::Message(uint32_t id, const char* buf, size_t size)
    : id_(id), len_(size) {
    data_.write(buf, size);
}

Message::Message() {}

Message::~Message() {}

uint32_t Message::getMsgID()const {
    return id_;
}

uint32_t Message::getMsgLen()const {
    return len_;
}

void Message::setMsgID(uint32_t id) {
    id_ = id;
}

void Message::setMsgLen(uint32_t len) {
    len_ = len;
}

//设置消息数组
void Message::setData(std::vector<char>& buf) {
    data_.write(buf.data(), buf.size());
}

//设置消息数组
void Message::setData(const char* buf, size_t size) {
    data_.write(buf, size);
}

//得到ByteBuffer
ByteBuffer<>& Message::getData() {
    return data_;
}

Message& Message::operator=(Message&& other) {
    id_ = other.id_;
    len_ = other.len_;
    this->data_ << other.data_;
    return *this;
}

}//namespace zinx_asio
