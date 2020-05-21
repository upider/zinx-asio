#ifndef BYTEBUFFERSTREAM_HPP
#define BYTEBUFFERSTREAM_HPP

#include <iostream>
#include <sstream>
#include <boost/asio/detail/config.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/basic_streambuf.hpp>

namespace zinx_asio { //namespace zinx_asio

template <typename Allocator = std::allocator<char>>
class ByteBufferStream: public std::streambuf {
    public:
        typedef boost::asio::const_buffer const_buffers_type;
        typedef boost::asio::mutable_buffer mutable_buffers_type;

        /******copy from boost::asio::basic_streambuf*******/
        ///返回使用的大小
        size_t size() const noexcept {
            return pptr() - gptr();
        }

        ///返回允许的最大序列
        size_t max_size()const {
            return max_size_;
        }

        ///返回容量
        std::size_t capacity() const noexcept {
            return buffer_.capacity();
        }

        const_buffers_type data() const noexcept {
            return boost::asio::buffer(boost::asio::const_buffer(gptr(),
                                       (pptr() - gptr()) * sizeof(char)));
        }

        mutable_buffers_type prepare(std::size_t n) {
            reserve(n);
            return boost::asio::buffer(boost::asio::mutable_buffer(
                                           pptr(), n * sizeof(char)));
        }

        void commit(std::size_t n) {
            n = std::min<std::size_t>(n, epptr() - pptr());
            pbump(static_cast<int>(n));
            setg(eback(), gptr(), pptr());
        }

        void consume(std::size_t n) {
            if (egptr() < pptr())
                setg(&buffer_[0], gptr(), pptr());
            if (gptr() + n > pptr())
                n = pptr() - gptr();
            gbump(static_cast<int>(n));
        }

    protected:
        /// Override std::streambuf behaviour.
        /**
         * Behaves according to the specification of @c std::streambuf::underflow().
         */
        int_type underflow() {
            if (gptr() < pptr()) {
                setg(&buffer_[0], gptr(), pptr());
                return traits_type::to_int_type(*gptr());
            }
            else {
                return traits_type::eof();
            }
        }

        /// Override std::streambuf behaviour.
        /**
         * Behaves according to the specification of @c std::streambuf::overflow(),
         * with the specialisation that @c std::length_error is thrown if appending
         * the character to the input sequence would require the condition
         * <tt>size() > max_size()</tt> to be true.
         */
        int_type overflow(int_type c) {
            if (!traits_type::eq_int_type(c, traits_type::eof())) {
                if (pptr() == epptr()) {
                    std::size_t buffer_size = pptr() - gptr();
                    if (buffer_size < max_size_ && max_size_ - buffer_size < buffer_delta) {
                        reserve(max_size_ - buffer_size);
                    }
                    else {
                        reserve(buffer_delta);
                    }
                }

                *pptr() = traits_type::to_char_type(c);
                pbump(1);
                return c;
            }

            return traits_type::not_eof(c);
        }

        void reserve(std::size_t n) {
            // Get current stream positions as offsets.
            std::size_t gnext = gptr() - &buffer_[0];
            std::size_t pnext = pptr() - &buffer_[0];
            std::size_t pend = epptr() - &buffer_[0];

            // Check if there is already enough space in the put area.
            if (n <= pend - pnext) {
                return;
            }

            // Shift existing contents of get area to start of buffer.
            if (gnext > 0) {
                pnext -= gnext;
                std::memmove(&buffer_[0], &buffer_[0] + gnext, pnext);
            }

            // Ensure buffer is large enough to hold at least the specified size.
            if (n > pend - pnext) {
                if (n <= max_size_ && pnext <= max_size_ - n) {
                    pend = pnext + n;
                    buffer_.resize((std::max<std::size_t>)(pend, 1));
                }
                else {
                    std::length_error ex("boost::asio::streambuf too long");
                    boost::asio::detail::throw_exception(ex);
                }
            }

            // Update stream positions.
            setg(&buffer_[0], &buffer_[0], &buffer_[0] + pnext);
            setp(&buffer_[0] + pnext, &buffer_[0] + pend);
        }

    protected:
        std::size_t buffer_delta{128};

    public:
        explicit ByteBufferStream(std::size_t initSize = 128,
                                  std::size_t bufDelta = 128,
                                  std::size_t maximum_size = (std::numeric_limits<std::size_t>::max)());
        template<typename T>
        explicit ByteBufferStream(ByteBufferStream<T>&);
        template<typename T>
        explicit ByteBufferStream(ByteBufferStream<T>&&);
        ~ByteBufferStream();

        ///清空
        ByteBufferStream<Allocator>& clear();
        ///复制
        template <typename T>
        ByteBufferStream<Allocator>& operator=(ByteBufferStream<T>&);
        ///复制
        template <typename T>
        ByteBufferStream<Allocator>& operator=(ByteBufferStream<T>&&);

    public:
        ///写入数据
        //写入int
        //移动读写指针
        ByteBufferStream& write(const void* val, size_t size);
        ///写入int8
        //移动读写指针
        ByteBufferStream& writeInt8(int8_t);
        ///写入int16
        //移动读写指针
        ByteBufferStream& writeInt16(int16_t);
        ///写入int32
        //移动读写指针
        ByteBufferStream& writeInt32(int32_t);
        ///写入int64
        //移动读写指针
        ByteBufferStream& writeInt64(int64_t);

        //写入uint
        ///写入uint8
        //移动读写指针
        ByteBufferStream& writeUint8(uint8_t);
        ///写入uint16
        //移动读写指针
        ByteBufferStream& writeUint16(uint16_t);
        ///写入uint32
        //移动读写指针
        ByteBufferStream& writeUint32(uint32_t);
        ///写入uint64
        //移动读写指针
        ByteBufferStream& writeUint64(uint64_t);

        ///写入float
        //移动读写指针
        ByteBufferStream& writeFloat(float);
        ///写入double
        //移动读写指针
        ByteBufferStream& writeDouble(double);

        ///读出数据
        //读出int
        //移动读写指针
        ByteBufferStream& read(void* val, size_t size);
        ///读出int8
        //移动读写指针
        ByteBufferStream& readInt8(int8_t&);
        ///读出int16
        //移动读写指针
        ByteBufferStream& readInt16(int16_t&);
        ///读出int32
        ByteBufferStream& readInt32(int32_t&);
        ///读出int64
        //移动读写指针
        ByteBufferStream& readInt64(int64_t&);

        //读出uint
        ///读出uint8
        //移动读写指针
        ByteBufferStream& readUint8(uint8_t&);
        ///读出uint16
        //移动读写指针
        ByteBufferStream& readUint16(uint16_t&);
        ///读出uint32
        //移动读写指针
        ByteBufferStream& readUint32(uint32_t&);
        ///读出uint64
        //移动读写指针
        ByteBufferStream& readUint64(uint64_t&);

        ///读出float浮点数
        //移动读写指针
        ByteBufferStream& readFloat(float&);
        ///读出double
        //移动读写指针
        ByteBufferStream& readDouble(double&);

        //写入string
        ByteBufferStream& writeString(const std::string&);
        //读出string
        ByteBufferStream& readString(std::string&);

        //流式读出算数类型
        template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>
        ByteBufferStream & operator>>(T&val);
        //流式读入算数类型
        template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>
        ByteBufferStream & operator<<(T val);

        //从vector输入和输出
        ByteBufferStream& operator<<(const std::vector<char>&);
        ByteBufferStream& operator>>(std::vector<char>&);
        //从array输入和输出
        template<std::size_t N>
        ByteBufferStream& operator<<(const std::array<char, N>&);
        template<std::size_t N>
        ByteBufferStream& operator>>(std::array<char, N>&);
        ///operator<<
        //移动读写指针
        ByteBufferStream& operator<<(const std::string&);
        ///operator<<
        //移动读写指针
        ByteBufferStream& operator>>(std::string&);
        ///operator<<
        //移动读写指针
        template<typename T>
        ByteBufferStream& operator<<(ByteBufferStream<T>&);
        ///operator>>
        //移动读写指针
        template<typename T>
        ByteBufferStream& operator>>(ByteBufferStream<T>&);
        ///operator<<
        //移动读写指针
        template<typename T>
        ByteBufferStream& operator<<(boost::asio::basic_streambuf<T>&);
        ///operator>>
        //移动读写指针
        template<typename T>
        ByteBufferStream& operator>>(boost::asio::basic_streambuf<T>&);

        //返回std::string
        //不移动读写指针
        std::string toString();
        //toVector
        //不移动读写指针
        std::vector<char> toVector();

        //copyToRawBuffer
        ByteBufferStream& copyToRawBuffer(char*, std::size_t);
        //copyToString
        //不移动读写指针
        ByteBufferStream& copyToString(std::string&);
        //copyToVector
        //不移动读写指针
        ByteBufferStream& copyToVector(std::vector<char>&);
        //copyToArray
        //不移动读写指针
        template<std::size_t N>
        ByteBufferStream& copyToArray(std::array<char, N>&);

        ///operator<<
        //移动读写指针
        template <typename T>
        friend std::ostream& operator << (std::ostream&, const ByteBufferStream<T>&);
        ///operator>>
        //移动读写指针
        template <typename T>
        friend std::ostream& operator >> (std::istream&, const ByteBufferStream<T>&);

    private:
        std::size_t max_size_;
        std::vector<char, Allocator> buffer_;

        // Helper function to get the preferred size for reading data.
        friend std::size_t read_size_helper(
            ByteBufferStream& sb, std::size_t max_size) {
            return std::min<std::size_t>(
                       std::max<std::size_t>(512, sb.buffer_.capacity() - sb.size()),
                       std::min<std::size_t>(max_size, sb.max_size() - sb.size()));
        }
};

template <typename Allocator>
ByteBufferStream<Allocator>::ByteBufferStream(std::size_t initSize,
        std::size_t bufDelta,
        std::size_t maximum_size)
    : buffer_delta(bufDelta), max_size_(maximum_size), buffer_(initSize) {
    setg(&buffer_[0], &buffer_[0], &buffer_[0]);
    setp(&buffer_[0], &buffer_[0] + initSize);
}

template <typename Allocator>
template<typename T>
ByteBufferStream<Allocator>::ByteBufferStream(ByteBufferStream<T>&& other)
    : max_size_(other.max_size()) {
    auto cbt = other.data();
    buffer_.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_begin(cbt) + other.size());
    setg(&buffer_[0], &buffer_[0], &buffer_[0] + buffer_.size());
    setp(&buffer_[0] + buffer_.size(), &buffer_[0] + max_size_);
}

template <typename Allocator>
template<typename T>
ByteBufferStream<Allocator>::ByteBufferStream(ByteBufferStream<T>& other)
    : max_size_(other.max_size()) {
    auto cbt = other.data();
    buffer_.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    setg(&buffer_[0], &buffer_[0], &buffer_[0] + buffer_.size());
    setp(&buffer_[0] + buffer_.size(), &buffer_[0] + max_size_);
}

template <typename Allocator>
ByteBufferStream<Allocator>::~ByteBufferStream() {}

///清空
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::clear() {
    consume(buffer_.size());
    return *this;
}

///复制
template <typename Allocator>
template <typename T>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator=(ByteBufferStream<T>& other) {
    if (&other == (ByteBufferStream<T>*)this) {
        return *this;
    }
    auto cbt = other.data();
    buffer_.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    setg(&buffer_[0], &buffer_[0], &buffer_[0] + buffer_.size());
    setp(&buffer_[0] + buffer_.size(), &buffer_[0] + max_size_);
    return *this;
}

///复制
template <typename Allocator>
template <typename T>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator=(ByteBufferStream<T>&& other) {
    if (&other == (ByteBufferStream<T>*)this) {
        return *this;
    }
    auto cbt = other.data();
    buffer_.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    setg(&buffer_[0], &buffer_[0], &buffer_[0] + buffer_.size());
    setp(&buffer_[0] + buffer_.size(), &buffer_[0] + max_size_);
    return *this;
}

///写入数字
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::write(const void* val, std::size_t size) {
    //保证空间足够
    reserve(size);
    //拷贝数据
    std::copy_n((char*)(val), size, pptr());
    commit(size);
    return *this;
}

///写入int8
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeInt8(int8_t val) {
    return write(&val, sizeof(val));
}

///写入int16
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeInt16(int16_t val) {
    return write(&val, sizeof(val));
}

///写入int32
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeInt32(int32_t val) {
    return write(&val, sizeof(val));
}

///写入int64
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeInt64(int64_t val) {
    return write(&val, sizeof(val));
}

//写入uint
///写入uint8
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeUint8(uint8_t val) {
    return write(&val, sizeof(val));
}

///写入uint16
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeUint16(uint16_t val) {
    return write(&val, sizeof(val));
}

///写入uint32
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeUint32(uint32_t val) {
    return write(&val, sizeof(val));
}

///写入uint64
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeUint64(uint64_t val) {
    return write(&val, sizeof(val));
}

///写入float浮点数
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeFloat(float val) {
    return write(&val, sizeof(val));
}

///写入double
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeDouble(double val) {
    return write(&val, sizeof(val));
}

//写入string
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::writeString(const std::string & val) {
    return write(val.data(), val.size());
}

//读出int
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::read(void* val, size_t size) {
    std::copy_n(gptr(), size, (char*)(val));
    consume(size);
    return *this;
}


//读出string
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readString(std::string & val) {
    auto cbt = data();
    val.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    clear();
    return *this;
}

///读出int8
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readInt8(int8_t& val) {
    return read(&val, sizeof(int8_t));
}

///读出int16
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readInt16(int16_t &val) {
    return read(&val, sizeof(int16_t));
}

///读出int32
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readInt32(int32_t& val) {
    return read(&val, sizeof(int32_t));
}

///读出int64
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readInt64(int64_t& val) {
    return read(&val, sizeof(int64_t));
}

//读出uint
///读出uint8
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readUint8(uint8_t& val) {
    return read(&val, sizeof(uint8_t));
}

///读出uint16
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readUint16(uint16_t& val) {
    return read(&val, sizeof(uint16_t));
}

///读出uint32
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readUint32(uint32_t& val) {
    return read(&val, sizeof(uint32_t));
}

///读出uint64
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readUint64(uint64_t& val) {
    return read(&val, sizeof(uint64_t));
}

///读出float浮点数
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readFloat(float & val) {
    return read(&val, sizeof(float));
}

///读出double
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::readDouble(double & val) {
    return read(&val, sizeof(double));
}

//流式读出算数类型
template <typename Allocator>
template<typename T, typename >
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator>>(T & val) {
    return read(&val, sizeof(val));
}

//流式读入算数类型
template <typename Allocator>
template<typename T, typename >
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator<<(T val) {
    return write(&val, sizeof(val));
}

//从vector输入和输出
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator<<(const std::vector<char>& vec) {
    return write(vec.data(), vec.size());
}

template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator>>(std::vector<char>& vec) {
    auto cbt = data();
    vec.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    clear();
    return *this;
}

//从array输入和输出
template <typename Allocator>
template<std::size_t N>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator<<(const std::array<char, N>& arr) {
    return write(arr.data(), arr.size());
}

template <typename Allocator>
template<std::size_t N>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator>>(std::array<char, N>& arr) {
    auto cbt = data();
    std::size_t size = this->size() < N ? this->size() : N;
    std::copy_n(boost::asio::buffers_begin(cbt), size, arr.begin());
    consume(size);
    return *this;
}

///operator<<
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator<<(const std::string & str) {
    write(str.data(), str.size());
    return *this;
}

///operator>>
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator>>(std::string & str) {
    auto cbt = data();
    str.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    clear();
    return *this;
}

//变成std::string
template <typename Allocator>
std::string ByteBufferStream<Allocator>::toString() {
    auto cbt = data();
    return std::string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
}

//toVector
//不移动读写指针
template <typename Allocator>
std::vector<char> ByteBufferStream<Allocator>::toVector() {
    auto cbt = data();
    return std::vector<char>(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
}

//copyToRawBuffer
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::copyToRawBuffer(char* dest, std::size_t size) {
    auto cbt = data();
    std::size_t n = size < buffer_.size() ? size : buffer_.size();
    std::copy_n(boost::asio::buffers_begin(cbt), n, dest);
    return *this;
}

//copyToString
//不移动读写指针
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::copyToString(std::string & str) {
    auto cbt = data();
    str.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    return *this;
}

//copyToVector
//不移动读写指针
template <typename Allocator>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::copyToVector(std::vector<char>& vec) {
    auto cbt = data();
    vec.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    return *this;
}

//copyToArray
//不移动读写指针
template <typename Allocator>
template<std::size_t N>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::copyToArray(std::array<char, N>& arr) {
    std::size_t size = buffer_.size() < N ? buffer_.size() : N;
    auto cbt = data();
    std::copy(boost::asio::buffers_begin(cbt), boost::asio::buffers_begin(cbt) + size, arr.begin());
    return *this;
}

template <typename Allocator>
template<typename T>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator<<(ByteBufferStream<T>& other) {
    auto cbt = other.data();
    buffer_.assign(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
    setg(&buffer_[0], &buffer_[0], &buffer_[0]);
    setp(&buffer_[0], &buffer_[0] + buffer_.size());
    other.clear();
    return *this;
}

///operator>>
template <typename Allocator>
template<typename T>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator>>(ByteBufferStream<T>& other) {
    return other << (*this);
}

///operator<<
//移动读写指针
template <typename Allocator>
template<typename T>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator<<(boost::asio::basic_streambuf<T>& buf) {
    std::ostream os(this);
    os << &buf;
    return *this;
}

///operator>>
//移动读写指针
template <typename Allocator>
template<typename T>
ByteBufferStream<Allocator>& ByteBufferStream<Allocator>::operator>>(boost::asio::basic_streambuf<T>& buf) {
    std::istream is(this);
    is >> &buf;
    return *this;
}

///operator<<
template <typename T>
std::ostream& operator<<(std::ostream & out, ByteBufferStream<T>& byteBuf) {
    if (byteBuf.size() == 0) {
        return out;
    }
    return out << &byteBuf;
}

///operator>>
template <typename T>
std::ostream& operator >> (std::istream & in, ByteBufferStream<T>& byteBuf) {
    return in >> &byteBuf;
}

}//namespace zinx_asio
#endif /* BYTEBUFFERSTREAM_HPP */
