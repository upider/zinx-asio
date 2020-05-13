#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include <sstream>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/basic_streambuf.hpp>

namespace zinx_asio { //namespace zinx_asio

template <typename Allocator = std::allocator<char>>
class ByteBuffer {
    public:
        ///构造函数,MaxSize最大容量
        ByteBuffer(std::size_t maxSize = std::numeric_limits<std::size_t>::max());
        template<typename T>
        ByteBuffer(ByteBuffer<T>&, std::size_t maxSize = std::numeric_limits<std::size_t>::max());
        ~ByteBuffer();

        ///指针回退n字节
        std::size_t backBytes(std::size_t);
        ///清空
        ByteBuffer<Allocator>& clear();
        ///复制
        template <typename T>
        ByteBuffer<Allocator>& operator=(ByteBuffer<T>&);

        ///写入数据
        //写入int
        //移动读写指针
        ByteBuffer& write(const void* val, size_t size);
        ///写入int8
        //移动读写指针
        ByteBuffer& writeInt8(int8_t);
        ///写入int16
        //移动读写指针
        ByteBuffer& writeInt16(int16_t);
        ///写入int32
        //移动读写指针
        ByteBuffer& writeInt32(int32_t);
        ///写入int64
        //移动读写指针
        ByteBuffer& writeInt64(int64_t);

        //写入uint
        ///写入uint8
        //移动读写指针
        ByteBuffer& writeUint8(uint8_t);
        ///写入uint16
        //移动读写指针
        ByteBuffer& writeUint16(uint16_t);
        ///写入uint32
        //移动读写指针
        ByteBuffer& writeUint32(uint32_t);
        ///写入uint64
        //移动读写指针
        ByteBuffer& writeUint64(uint64_t);

        ///写入float
        //移动读写指针
        ByteBuffer& writeFloat(float);
        ///写入double
        //移动读写指针
        ByteBuffer& writeDouble(double);

        ///读出数据
        //读出int
        //移动读写指针
        ByteBuffer& read(void* val, size_t size);
        ///读出int8
        //移动读写指针
        ByteBuffer& readInt8(int8_t&);
        ///读出int16
        //移动读写指针
        ByteBuffer& readInt16(int16_t&);
        ///读出int32
        ByteBuffer& readInt32(int32_t&);
        ///读出int64
        //移动读写指针
        ByteBuffer& readInt64(int64_t&);

        //读出uint
        ///读出uint8
        //移动读写指针
        ByteBuffer& readUint8(uint8_t&);
        ///读出uint16
        //移动读写指针
        ByteBuffer& readUint16(uint16_t&);
        ///读出uint32
        //移动读写指针
        ByteBuffer& readUint32(uint32_t&);
        ///读出uint64
        //移动读写指针
        ByteBuffer& readUint64(uint64_t&);

        ///读出float浮点数
        //移动读写指针
        ByteBuffer& readFloat(float&);
        ///读出double
        //移动读写指针
        ByteBuffer& readDouble(double&);

        //流式读出算数类型
        template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>
        ByteBuffer & operator>>(T & val);
        //流式读入算数类型
        template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>
        ByteBuffer & operator<<(T & val);

        //从vector输入和输出
        ByteBuffer& operator<<(const std::vector<char>&);
        ByteBuffer& operator>>(std::vector<char>&);
        //从array输入和输出
        template<std::size_t N>
        ByteBuffer& operator<<(const std::array<char, N>&);
        template<std::size_t N>
        ByteBuffer& operator>>(std::array<char, N>&);
        ///operator<<
        //移动读写指针
        ByteBuffer& operator<<(const std::string&);
        ///operator<<
        //移动读写指针
        ByteBuffer& operator>>(std::string&);
        ///operator<<
        //移动读写指针
        template<typename T>
        ByteBuffer& operator<<(ByteBuffer<T>&);
        ///operator>>
        //移动读写指针
        template<typename T>
        ByteBuffer& operator>>(ByteBuffer<T>&);

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

        //返回std::string
        //不移动读写指针
        std::string toString();
        //toVector
        //不移动读写指针
        std::vector<char> toVector();

        //copyToString
        //不移动读写指针
        ByteBuffer& copyToString(std::string&);
        //copyToVector
        //不移动读写指针
        ByteBuffer& copyToVector(std::vector<char>&);
        //copyToArray
        //不移动读写指针
        template<std::size_t N>
        ByteBuffer& copyToArray(std::array<char, N>&);

        ///operator<<
        //移动读写指针
        template <typename T>
        friend std::ostream& operator << (std::ostream&, const ByteBuffer<T>&);
        ///operator>>
        //移动读写指针
        template <typename T>
        friend std::ostream& operator >> (std::istream&, const ByteBuffer<T>&);

    private:
        boost::asio::basic_streambuf<Allocator> data_;
};

template <typename Allocator>
ByteBuffer<Allocator>::ByteBuffer(std::size_t maxSize) {}


template <typename Allocator>
template<typename T>
ByteBuffer<Allocator>::ByteBuffer(ByteBuffer<T>& other, std::size_t maxSize)
    : data_(maxSize) {
    std::ostreambuf_iterator<char> osit(&data_);
    auto cbt = other.buf().data();
    std::copy(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt), osit);
}

template <typename Allocator>
ByteBuffer<Allocator>::~ByteBuffer() {}

///指针回退n字节
template <typename Allocator>
std::size_t ByteBuffer<Allocator>::backBytes(std::size_t size) {
    size_t i = 0;
    for (; i < size; ++i) {
        if (data_.sungetc() == EOF) {
            return i;
        }
    }
    return i;
}

///清空
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::clear() {
    data_.consume(data_.size());
    return *this;
}

///复制
template <typename Allocator>
template <typename T>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator=(ByteBuffer<T>& other) {
    std::ostreambuf_iterator<char> osit(&data_);
    auto cbt = other.buf().data();
    std::copy(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt), osit);
    return *this;
}

///写入数字
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::write(const void* val, std::size_t size) {
    std::ostream os(&data_);
    os.write((char*)(val), size);
    return *this;
}

///写入int8
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeInt8(int8_t val) {
    return write(&val, sizeof(val));
}

///写入int16
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeInt16(int16_t val) {
    return write(&val, sizeof(val));
}

///写入int32
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeInt32(int32_t val) {
    return write(&val, sizeof(val));
}

///写入int64
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeInt64(int64_t val) {
    return write(&val, sizeof(val));
}

//写入uint
///写入uint8
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeUint8(uint8_t val) {
    return write(&val, sizeof(val));
}

///写入uint16
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeUint16(uint16_t val) {
    return write(&val, sizeof(val));
}

///写入uint32
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeUint32(uint32_t val) {
    return write(&val, sizeof(val));
}

///写入uint64
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeUint64(uint64_t val) {
    return write(&val, sizeof(val));
}

///写入float浮点数
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeFloat(float val) {
    return write(&val, sizeof(val));
}

///写入double
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::writeDouble(double val) {
    return write(&val, sizeof(val));
}

//读出int
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::read(void* val, size_t size) {
    std::istream is(&data_);
    is.read((char*)(val), size);
    //data_.commit(size);
    //data_.consume(size);
    return *this;
}

///读出int8
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readInt8(int8_t& val) {
    return read(&val, sizeof(int8_t));
}

///读出int16
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readInt16(int16_t &val) {
    return read(&val, sizeof(int16_t));
}

///读出int32
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readInt32(int32_t& val) {
    return read(&val, sizeof(int32_t));
}

///读出int64
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readInt64(int64_t& val) {
    return read(&val, sizeof(int64_t));
}

//读出uint
///读出uint8
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readUint8(uint8_t& val) {
    return read(&val, sizeof(uint8_t));
}

///读出uint16
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readUint16(uint16_t& val) {
    return read(&val, sizeof(uint16_t));
}

///读出uint32
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readUint32(uint32_t& val) {
    return read(&val, sizeof(uint32_t));
}

///读出uint64
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readUint64(uint64_t& val) {
    return read(&val, sizeof(uint64_t));
}

///读出float浮点数
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readFloat(float& val) {
    return read(&val, sizeof(float));
}

///读出double
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::readDouble(double& val) {
    return read(&val, sizeof(double));
}

//流式读出算数类型
template <typename Allocator>
template<typename T, typename >
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator>>(T & val) {
    return read(&val, sizeof(val));
}

//流式读入算数类型
template <typename Allocator>
template<typename T, typename >
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator<<(T & val) {
    return write(&val, sizeof(val));
}

//从vector输入和输出
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator<<(const std::vector<char>& vec) {
    std::ostreambuf_iterator<char> osit(&data_);
    std::copy(vec.cbegin(), vec.cend(), osit);
    return *this;
}

template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator>>(std::vector<char>& vec) {
    uint32_t size = data_.size();
    auto cbt = data_.data();
    vec.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    data_.consume(size);
    return *this;
}

//从array输入和输出
template <typename Allocator>
template<std::size_t N>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator<<(const std::array<char, N>& arr) {
    std::ostreambuf_iterator<char> osit(&data_);
    std::copy(arr.begin(), arr.end(), osit);
    return *this;
}

template <typename Allocator>
template<std::size_t N>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator>>(std::array<char, N>& arr) {
    auto cbt = data_.data();
    std::size_t size = data_.size() < N ? data_.size() : N;
    std::copy(boost::asio::buffers_begin(cbt), boost::asio::buffers_begin(cbt) + size, arr.begin());
    data_.consume(size);
    return *this;
}

///operator<<
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator<<(const std::string& str) {
    std::ostream os(&data_);
    os << str;
    return *this;
}

///operator>>
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator>>(std::string& str) {
    uint32_t size = data_.size();
    auto cbt = data_.data();
    //不能用>>,会从空格分开
    str.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    data_.consume(size);
    return *this;
}


template <typename Allocator>
size_t ByteBuffer<Allocator>::maxSize()const {
    return data_.max_size();
}

template <typename Allocator>
size_t ByteBuffer<Allocator>::size()const {
    return data_.size();
}

template <typename Allocator>
std::size_t ByteBuffer<Allocator>::capacity() const {
    return data_.capacity();
}

///准备空间
template <typename Allocator>
void ByteBuffer<Allocator>::prepare(std::size_t size) {
    data_.prepare(size);
}

///消耗掉已经发送的数据
template <typename Allocator>
void ByteBuffer<Allocator>::consume(std::size_t size) {
    data_.consume(size);
}

///放进已经接受的数据
template <typename Allocator>
void ByteBuffer<Allocator>::commit(std::size_t size) {
    data_.commit(size);
}

///返回streambuf
template <typename Allocator>
boost::asio::basic_streambuf<Allocator>& ByteBuffer<Allocator>::buf() {
    return data_;
}

//变成std::string
template <typename Allocator>
std::string ByteBuffer<Allocator>::toString() {
    auto cbt = data_.data();
    return std::string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
}

//toVector
//不移动读写指针
template <typename Allocator>
std::vector<char> ByteBuffer<Allocator>::toVector() {
    auto cbt = data_.data();
    return std::vector<char>(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
}

//copyToString
//不移动读写指针
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::copyToString(std::string& str) {
    auto cbt = data_.data();
    //不能用>>,会从空格分开
    str.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    return *this;
}

//copyToVector
//不移动读写指针
template <typename Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::copyToVector(std::vector<char>& vec) {
    auto cbt = data_.data();
    vec.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    return *this;
}

//copyToArray
//不移动读写指针
template <typename Allocator>
template<std::size_t N>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::copyToArray(std::array<char, N>& arr) {
    auto cbt = data_.data();
    std::size_t size = data_.size() < N ? data_.size() : N;
    std::copy(boost::asio::buffers_begin(cbt), boost::asio::buffers_begin(cbt) + size, arr.begin());
    return *this;
}

///operator<<
template <typename Allocator>
template<typename T>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator<<(ByteBuffer<T>& other) {
    std::ostream os(&data_);
    os << &other.buf();
    return *this;
}

///operator>>
template <typename Allocator>
template<typename T>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator>>(ByteBuffer<T>& other) {
    std::istream is(&data_);
    is >> &other.buf();
    return *this;
}

///operator<<
template <typename T>
std::ostream& operator<<(std::ostream& out, ByteBuffer<T>& byteBuf) {
    if (byteBuf.size() == 0) {
        return out;
    }
    return out << &byteBuf.buf();
}

///operator>>
template <typename T>
std::ostream& operator >> (std::istream& in, ByteBuffer<T>& byteBuf) {
    return in >> &byteBuf.buf();
}

}//namespace zinx_asio
#endif /* BYTEBUFFER_HPP */
