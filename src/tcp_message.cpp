#include <iostream>
#include <cstring>
#include "tcp_message.hpp"

namespace zinx_asio {//namespace zinx_asio

TCPMessage::TCPMessage(): data_() {}

TCPMessage::TCPMessage(uint32_t id, const char* buf, size_t len) {
    std::memcpy(head_, &len, 4);
    std::memcpy(head_ + 4, &id, 4);
    data_.write(buf, len);
}

TCPMessage::TCPMessage(uint32_t id, std::size_t len): data_() {
    std::memcpy(head_, &len, 4);
    std::memcpy(head_ + 4, &id, 4);
}

TCPMessage::TCPMessage(TCPMessage&& other)
    : data_(std::move(other.data_)) {
    std::swap(head_, other.head_);
}

TCPMessage::TCPMessage(const TCPMessage& other)
    : data_(other.data_) {
    std::memcpy(head_, other.head_, 8);
}

TCPMessage::~TCPMessage() {}

TCPMessage& TCPMessage::operator=(const TCPMessage& other) {
    if (this == &other) {
        return *this;
    }
    std::memcpy(head_, other.head_, 8);
    this->data_ = other.data_;
    return *this;
}

TCPMessage& TCPMessage::operator=(TCPMessage&& other) {
    if (this == &other) {
        return *this;
    }
    std::swap(head_, other.head_);
    this->data_ = std::move(other.data_);
    return *this;
}

uint32_t TCPMessage::getMsgID()const {
    return head_[4];
}

uint32_t TCPMessage::getMsgLen()const {
    return head_[0];
}

void TCPMessage::setMsgID(uint32_t id) {
    std::memcpy(head_ + 4, &id, 4);
}

void TCPMessage::setMsgLen(uint32_t len) {
    std::memcpy(head_, &len, 4);
}

//-----------------设置消息数组------------------
void TCPMessage::setContent(const std::vector<char>& buf) {
    data_.write(buf.data(), buf.size());
}

void TCPMessage::setContent(const std::string& buf) {
    data_.write(buf.data(), buf.size());
}

void TCPMessage::setContent(const char* buf, size_t size) {
    data_.write(buf, size);
}

//得到ByteBuffer
ByteBuffer<>& TCPMessage::bufferRef() {
    return data_;
}

std::string TCPMessage::contentToString() {
    return data_.toString();
}

//适配asio buffer
std::list<boost::asio::const_buffer> TCPMessage::data() {
    std::list<boost::asio::const_buffer> list;
    list.emplace_back(&head_[0], 8);
    list.push_back(data_.data());
    return list;
}

void TCPMessage::commit(std::size_t n) {
    data_.commit(n);
}

void TCPMessage::consume(std::size_t n) {
    data_.consume(n);
}

}//namespace zinx_asio
