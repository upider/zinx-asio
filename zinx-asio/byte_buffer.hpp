#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include <boost/asio/basic_streambuf.hpp>

namespace zinx_asio { //namespace zinx_asio

template <typename Allocator = std::allocator<char>>
class ByteBuffer {
    public:
        ///构造函数,MaxSize最大容量
        ByteBuffer(std::size_t maxSize = std::numeric_limits<std::size_t>::max());
        virtual ~ByteBuffer();

        ///写入数据
        //写入int
        ByteBuffer<>& write(const void* val, size_t size);
        ///写入int8
        ByteBuffer<>& writeInt8(int8_t);
        ///写入int16
        ByteBuffer<>& writeInt16(int16_t);
        ///写入int32
        ByteBuffer<>& writeInt32(int32_t);
        ///写入int64
        ByteBuffer<>& writeInt64(int64_t);

        //写入uint
        ///写入uint8
        ByteBuffer<>& writeUint8(uint8_t);
        ///写入uint16
        ByteBuffer<>& writeUint16(uint16_t);
        ///写入uint32
        ByteBuffer<>& writeUint32(uint32_t);
        ///写入uint64
        ByteBuffer<>& writeUint64(uint64_t);

        ///写入float
        ByteBuffer<>& writeFloat(float);
        ///写入double
        ByteBuffer<>& writeDouble(double);

        ///读出数据
        //读出int
        ByteBuffer<>& read(const void* val, size_t size);
        ///读出int8
        ByteBuffer<>& readInt8(int8_t&);
        ///读出int16
        ByteBuffer<>& readInt16(int16_t&);
        ///读出int32
        ByteBuffer<>& readInt32(int32_t&);
        ///读出int64
        ByteBuffer<>& readInt64(int64_t&);

        //读出uint
        ///读出uint8
        ByteBuffer<>& readUint8(uint8_t&);
        ///读出uint16
        ByteBuffer<>& readUint16(uint16_t&);
        ///读出uint32
        ByteBuffer<>& readUint32(uint32_t&);
        ///读出uint64
        ByteBuffer<>& readUint64(uint64_t&);

        ///读出float浮点数
        ByteBuffer<>& readFloat(float&);
        ///读出double
        ByteBuffer<>& readDouble(double&);

        ///读出int8
        ByteBuffer<>& operator>>(int8_t&);
        ///读出int16
        ByteBuffer<>& operator>>(int16_t&);
        ///读出int32
        ByteBuffer<>& operator>>(int32_t&);
        ///读出int64
        ByteBuffer<>& operator>>(int64_t&);

        //读出uint
        ///读出uint8
        ByteBuffer<>& operator>>(uint8_t&);
        ///读出uint16
        ByteBuffer<>& operator>>(uint16_t&);
        ///读出uint32
        ByteBuffer<>& operator>>(uint32_t&);
        ///读出uint64
        ByteBuffer<>& operator>>(uint64_t&);

        ///读出float浮点数
        ByteBuffer<>& operator>>(float&);
        ///读出double
        ByteBuffer<>& operator>>(double&);

        ///返回允许的最大序列
        std::size_t maxSize() const;
        ///返回使用的大小
        std::size_t size() const;
        ///返回容量
        std::size_t capacity() const;
        ///准备空间
        void prepare(std::size_t);
        ///消耗掉已经发送的数据
        void consume(std::size_t);
        ///放进已经接受的数据
        void commit(std::size_t);
        ///放回streambuf
        boost::asio::basic_streambuf<Allocator>& buf();

        /// The type used to represent the input sequence as a list of buffers.
        //返回boost::asio::const_buffer
        boost::asio::basic_streambuf<>::const_buffers_type data() const;

        //变成std::string
        std::string toString();

        ///operator<<
        ByteBuffer<>& operator<<(const std::string&);
        ///operator<<
        ByteBuffer<>& operator>>(std::string&);
        ///operator<<
        ByteBuffer<>& operator<<(ByteBuffer<>&);
        ///operator>>
        ByteBuffer<>& operator>>(ByteBuffer<>&);

        ///operator<<
        template <typename T>
        friend std::ostream& operator << (std::ostream&, const ByteBuffer<T>&);
        ///operator>>
        template <typename T>
        friend std::ostream& operator >> (std::istream&, const ByteBuffer<T>&);

    private:
        boost::asio::basic_streambuf<Allocator> data_;
};

///operator<<
template <typename T>
std::ostream& operator<<(std::ostream& out, ByteBuffer<T>& byteBuf) {
    return out << &byteBuf.buf();
}

///operator>>
template <typename T>
std::ostream& operator >> (std::istream& in, ByteBuffer<T>& byteBuf) {
    return in >> &byteBuf.buf();
}

}//namespace zinx_asio
#endif /* BYTEBUFFER_HPP */
