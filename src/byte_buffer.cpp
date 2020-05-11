#include <iostream>
#include <sstream>
#include "byte_buffer.hpp"

namespace zinx_asio { //namespace zinx_asio

template <>
ByteBuffer<>::ByteBuffer(std::size_t maxSize) {}

template <>
ByteBuffer<>::~ByteBuffer() {}

///写入数字
template <>
ByteBuffer<>& ByteBuffer<>::write(const void* val, std::size_t size) {
    std::ostream os(&data_);
    os.write((char*)(val), size);
    return *this;
}

///写入int8
template <>
ByteBuffer<>& ByteBuffer<>::writeInt8(int8_t val) {
    return write(&val, sizeof(val));
}

///写入int16
template <>
ByteBuffer<>& ByteBuffer<>::writeInt16(int16_t val) {
    return write(&val, sizeof(val));
}

///写入int32
template <>
ByteBuffer<>& ByteBuffer<>::writeInt32(int32_t val) {
    return write(&val, sizeof(val));
}

///写入int64
template <>
ByteBuffer<>& ByteBuffer<>::writeInt64(int64_t val) {
    return write(&val, sizeof(val));
}

//写入uint
///写入uint8
template <>
ByteBuffer<>& ByteBuffer<>::writeUint8(uint8_t val) {
    return write(&val, sizeof(val));
}

///写入uint16
template <>
ByteBuffer<>& ByteBuffer<>::writeUint16(uint16_t val) {
    return write(&val, sizeof(val));
}

///写入uint32
template <>
ByteBuffer<>& ByteBuffer<>::writeUint32(uint32_t val) {
    return write(&val, sizeof(val));
}

///写入uint64
template <>
ByteBuffer<>& ByteBuffer<>::writeUint64(uint64_t val) {
    return write(&val, sizeof(val));
}

///写入float浮点数
template <>
ByteBuffer<>& ByteBuffer<>::writeFloat(float val) {
    return write(&val, sizeof(val));
}

///写入double
template <>
ByteBuffer<>& ByteBuffer<>::writeDouble(double val) {
    return write(&val, sizeof(val));
}

//读出int
template <>
ByteBuffer<>& ByteBuffer<>::read(const void* val, size_t size) {
    std::istream is(&data_);
    is.read((char*)(val), size);
    return *this;
}

///读出int8
template <>
ByteBuffer<>& ByteBuffer<>::readInt8(int8_t& val) {
    return read(&val, sizeof(int8_t));
}

///读出int16
template <>
ByteBuffer<>& ByteBuffer<>::readInt16(int16_t &val) {
    return read(&val, sizeof(int16_t));
}

///读出int32
template <>
ByteBuffer<>& ByteBuffer<>::readInt32(int32_t& val) {
    return read(&val, sizeof(int32_t));
}

///读出int64
template <>
ByteBuffer<>& ByteBuffer<>::readInt64(int64_t& val) {
    return read(&val, sizeof(int64_t));
}

//读出uint
///读出uint8
template <>
ByteBuffer<>& ByteBuffer<>::readUint8(uint8_t& val) {
    return read(&val, sizeof(uint8_t));
}

///读出uint16
template <>
ByteBuffer<>& ByteBuffer<>::readUint16(uint16_t& val) {
    return read(&val, sizeof(uint16_t));
}

///读出uint32
template <>
ByteBuffer<>& ByteBuffer<>::readUint32(uint32_t& val) {
    return read(&val, sizeof(uint32_t));
}

///读出uint64
template <>
ByteBuffer<>& ByteBuffer<>::readUint64(uint64_t& val) {
    return read(&val, sizeof(uint64_t));
}

///读出float浮点数
template <>
ByteBuffer<>& ByteBuffer<>::readFloat(float& val) {
    return read(&val, sizeof(float));
}

///读出double
template <>
ByteBuffer<>& ByteBuffer<>::readDouble(double& val) {
    return read(&val, sizeof(double));
}

///读出int8
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(int8_t& val) {
    read(&val, sizeof(int8_t));
    return *this;
}

///读出int16
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(int16_t& val) {
    read(&val, sizeof(int16_t));
    return *this;
}

///读出int32
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(int32_t& val) {
    read(&val, sizeof(int32_t));
    return *this;
}

///读出int64
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(int64_t& val) {
    read(&val, sizeof(int64_t));
    return *this;
}

//读出uint
///读出uint8
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(uint8_t& val) {
    read(&val, sizeof(uint8_t));
    return *this;
}

///读出uint16
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(uint16_t& val) {
    read(&val, sizeof(uint16_t));
    return *this;
}

///读出uint32
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(uint32_t& val) {
    read(&val, sizeof(uint32_t));
    return *this;
}

///读出uint64
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(uint64_t& val) {
    read(&val, sizeof(uint64_t));
    return *this;
}

///读出float浮点数
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(float& val) {
    read(&val, sizeof(float));
    return *this;
}

///读出double
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(double& val) {
    read(&val, sizeof(double));
    return *this;
}

template <>
size_t ByteBuffer<>::maxSize()const {
    return data_.max_size();
}

template <>
size_t ByteBuffer<>::size()const {
    return data_.size();
}

template <>
std::size_t ByteBuffer<>::capacity() const {
    return data_.capacity();
}

///准备空间
template <>
void ByteBuffer<>::prepare(std::size_t size) {
    data_.prepare(size);
}

///消耗掉已经发送的数据
template <>
void ByteBuffer<>::consume(std::size_t size) {
    data_.consume(size);
}

///放进已经接受的数据
template <>
void ByteBuffer<>::commit(std::size_t size) {
    data_.commit(size);
}

///返回streambuf
template <>
boost::asio::basic_streambuf<>& ByteBuffer<>::buf() {
    return data_;
}

//变成std::string
template <>
std::string ByteBuffer<>::toString() {
    std::ostringstream os;
    os << &data_;
    return os.str();
}

template <>
boost::asio::basic_streambuf<>::const_buffers_type ByteBuffer<>::data() const {
    return data_.data();
}

///operator<<
template <>
ByteBuffer<>& ByteBuffer<>::operator<<(const std::string& str) {
    std::ostream os(&data_);
    os << str;
    return *this;
}

///operator>>
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(std::string& str) {
    std::istream is(&data_);
    is >> str;
    return *this;
}

///operator<<
template <>
ByteBuffer<>& ByteBuffer<>::operator<<(ByteBuffer<>& other) {
    std::ostream os(&data_);
    os << &other.buf();
    return *this;
}

///operator>>
template <>
ByteBuffer<>& ByteBuffer<>::operator>>(ByteBuffer<>& other) {
    std::istream is(&data_);
    is >> &other.buf();
    return *this;
}

}//namespace zinx_asio
