#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP

#include <algorithm>
#include <iostream>
#include <boost/asio/buffer.hpp>

namespace zinx_asio { //namespace zinx_asio

template<typename Allocator = std::allocator<char>>
class ByteBuffer {
    public:
        using const_buffers_type = boost::asio::const_buffer;
        using mutable_buffers_type = boost::asio::mutable_buffer;
        explicit ByteBuffer(std::size_t initSize = 128,
                            std::size_t max_size = std::numeric_limits<std::size_t>::max());

        template<typename T>
        explicit ByteBuffer(const std::vector<T>& vec,
                            std::size_t max_size = std::numeric_limits<std::size_t>::max());

        explicit ByteBuffer(const std::string& str,
                            std::size_t max_size = std::numeric_limits<std::size_t>::max());

        template<typename T, std::size_t N>
        explicit ByteBuffer(const std::array<T, N>& vec,
                            std::size_t max_size = std::numeric_limits<std::size_t>::max());

        template<typename T>
        explicit ByteBuffer(const ByteBuffer<T>&);
        template<typename T>
        explicit ByteBuffer(ByteBuffer<T>&&);

        template<typename T>
        ByteBuffer& operator=(const ByteBuffer<T>&);
        template<typename T>
        ByteBuffer& operator=(ByteBuffer<T>&&);

        ~ByteBuffer();

    public:
        //dynamic_buffer_v1 requirements
        std::size_t size() const noexcept {
            return distance(in_, out_);
        }

        std::size_t max_size() const noexcept {
            return max_size_;
        }

        std::size_t capacity() const noexcept {
            return distance(begin_, end_);
        }

        const_buffers_type data() const noexcept {
            return {in_, distance(in_, out_)};
        }

        mutable_buffers_type prepare(std::size_t n) {
            auto const len = size();
            if(len > max_size_ || n > (max_size_ - len))
                BOOST_THROW_EXCEPTION(std::length_error{"ByteBuffer too long"});
            if(n <= distance(out_, end_)) {
                // existing capacity is sufficient
                last_ = out_ + n;
                return{out_, n};
            }
            if(n <= capacity() - len) {
                // after a memmove,
                // existing capacity is sufficient
                if(len > 0) {
                    std::memmove(begin_, in_, len);
                }
                in_ = begin_;
                out_ = in_ + len;
                last_ = out_ + n;
                return {out_, n};
            }
            // allocate a new buffer
            auto const new_size = std::min<std::size_t>(max_size_,
                                  (std::max<std::size_t>)(2 * len, len + n));
            auto p = allocator_.allocate(new_size);
            if(begin_) {
                std::memcpy(p, in_, len);
                allocator_.deallocate(begin_, capacity());
            }
            begin_ = p;
            in_ = begin_;
            out_ = in_ + len;
            last_ = out_ + n;
            end_ = begin_ + new_size;
            return {out_, n};
        }

        void commit(std::size_t n) noexcept {
            out_ += std::min(n, distance(out_, last_));
        }

        void consume(std::size_t n) {
            //如果缓存全部被读取
            if(n >= distance(in_, out_)) {
                in_ = begin_;
                out_ = begin_;
                return;
            }
            //否则
            in_ += n;
        }
        //dynamic_buffer_v1 requirements

        //返回缓冲区(T*)头指针和长度,方便直接计算,避免数据拷贝
        template<typename T = char>
        std::pair<T*, std::size_t> getRawBuffer();
        //以vector返回缓冲区拷贝
        template<typename T>
        std::vector<T> toVector();
        //以string返回缓冲区拷贝
        std::string toString();
        //重置指针
        void clear()noexcept;
        //可写区域大小
        std::size_t writeableSize()const noexcept;

        //写入读出数字类型
        //write方法写入的数字最好有明确的类型,直接写入数字默认为uint32
        template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>
        ByteBuffer & write(T val);
        template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>
        ByteBuffer & read(T& val);
        //写入读出string
        ByteBuffer& write(const std::string& val);
        ByteBuffer& read(std::string& val);
        //写入读出vector
        template<typename T>
        ByteBuffer& write(const std::vector<T>& val);
        template<typename T>
        ByteBuffer& read(std::vector<T>& val);
        //写入读出array
        template<typename T, std::size_t N>
        ByteBuffer& write(const std::array<T, N>& val);
        template<typename T, std::size_t N>
        ByteBuffer& read(std::array<T, N>& val);

        //基本写入和读出方法
        ByteBuffer & write(const void* val, std::size_t size);
        ByteBuffer & read(void* val, std::size_t size);

    private:
        //预留空间
        void reserve(std::size_t n);
        //拷贝
        template<typename T>
        void copyFrom(ByteBuffer<T> const& other);
        //计算指针距离
        static std::size_t distance(char const* first, char const* last)noexcept;

    private:
        template<class OtherAlloc>
        friend class ByteBuffer;

        Allocator allocator_;
        std::size_t max_size_;
        //可读区域in-->out
        //可写区域out-->last
        //总容量begin-->end
        char* begin_;
        char* in_;
        char* out_;
        char* last_;
        char* end_;
};

//------------------------------------------------------------------------------

template<typename Allocator>
ByteBuffer<Allocator>::ByteBuffer(std::size_t initSize,
                                  std::size_t max_size)
    : max_size_(max_size), begin_(nullptr),
      in_(nullptr), out_(nullptr),
      last_(nullptr), end_(nullptr) {
    begin_ = allocator_.allocate(initSize);
    in_ = begin_;
    out_ = begin_;
    end_ = begin_ + initSize;
    last_ = end_;
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>::ByteBuffer(const std::vector<T>& vec, std::size_t max_size) {
    std::size_t n = vec.size() * sizeof(T);
    begin_ = allocator_.allocate(n * 2);
    std::copy_n(reinterpret_cast<char*>(vec.data()), n, begin_);
    in_ = begin_;
    end_ = begin_ + n * 2;
    last_ = begin_ + n;
    out_ = begin_ + n;
}

template<typename Allocator>
ByteBuffer<Allocator>::ByteBuffer(const std::string& str,
                                  std::size_t max_size) {
    std::size_t n = str.size();
    begin_ = allocator_.allocate(n * 2);
    std::copy_n(str.data(), n, begin_);
    in_ = begin_;
    out_ = begin_ + n;
    last_ = begin_ + n;
    end_ = begin_ + n * 2;
}

template<typename Allocator>
template<typename T, std::size_t N>
ByteBuffer<Allocator>::ByteBuffer(const std::array<T, N>& vec,
                                  std::size_t max_size) {
    std::size_t n = N * sizeof(T);
    begin_ = allocator_.allocate(n * 2);
    std::copy_n(reinterpret_cast<char*>(vec.data()), n, begin_);
    in_ = begin_;
    out_ = begin_ + n;
    last_ = begin_ + n;
    end_ = begin_ + n * 2;
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>::ByteBuffer(const ByteBuffer<T>& other)
    : max_size_(other.max_size_), begin_(nullptr),
      in_(nullptr), out_(nullptr),
      last_(nullptr), end_(nullptr) {
    copyFrom(other);
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>::ByteBuffer(ByteBuffer<T>&& other)
    : max_size_(other.max_size_), begin_(nullptr),
      in_(nullptr), out_(nullptr),
      last_(nullptr), end_(nullptr) {

    if(this == &other)
        return;

    std::swap(begin_, other.begin_);
    std::swap(last_, other.last_);
    std::swap(end_, other.end_);
    std::swap(in_, other.in_);
    std::swap(out_, other.out_);

    other.begin_ = other.allocator_.allocate(this->capacity());
    other.in_ = other.begin_;
    other.out_ = other.begin_;
    other.last_ = other.begin_;
    other.end_ = other.begin_ + this->capacity();
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator=(const ByteBuffer<T>& other) {
    if(this == &other)
        return *this;
    max_size_ = other.max_size_;
    copyFrom(other);
    return *this;
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::operator=(ByteBuffer<T>&& other) {
    if(this == &other)
        return *this;

    if (begin_) {
        allocator_.deallocate(begin_, this->capacity());
        begin_ = nullptr;
    }

    in_ = other.in_;
    out_ = other.out_;
    last_ = other.last_;
    begin_ = other.begin_;
    end_ = other.end_;
    max_size_ = other.max_size_;

    return *this;
}

template<typename Allocator>
ByteBuffer<Allocator>::~ByteBuffer() {
    if (begin_) {
        allocator_.deallocate(begin_, this->capacity());
        begin_ = nullptr;
    }
}

//返回缓冲区(T*)头指针和长度,方便直接计算,避免数据拷贝
template<typename Allocator>
template<typename T>
std::pair<T*, std::size_t> ByteBuffer<Allocator>::getRawBuffer() {
    return {reinterpret_cast<T*>(in_), size() / sizeof(T)};
}

template<typename Allocator>
template<typename T>
std::vector<T> ByteBuffer<Allocator>::toVector() {
    return std::vector<T>(reinterpret_cast<T*>(in_),
                          reinterpret_cast<T*>(out_));
}

template<typename Allocator>
std::string ByteBuffer<Allocator>::toString() {
    return std::string(in_, out_);
}

template<typename Allocator>
std::size_t ByteBuffer<Allocator>::distance(char const* first, char const* last) noexcept {
    return static_cast<std::size_t>(last - first);
}

template<typename Allocator>
void ByteBuffer<Allocator>::clear() noexcept {
    in_ = begin_;
    out_ = begin_;
    last_ = begin_;
}

//可写区域大小
template<class Allocator>
std::size_t ByteBuffer<Allocator>::writeableSize() const noexcept {
    return distance(out_, end_);
}

template<class Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::write(const void* val, std::size_t size) {
    reserve(size);
    std::memcpy(out_, (char*)val, size);
    commit(size);
    return *this;
}

template<class Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::read(void* val, std::size_t size) {
    size = size <= this->size() ? size : this->size();
    std::memcpy(static_cast<char*>(val), in_, size);
    consume(size);
    return *this;
}

//写入读出数字类型
template<typename Allocator>
template<typename T, typename>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::write(T val) {
    return write(&val, sizeof(T));
}

template<typename Allocator>
template<typename T, typename>
ByteBuffer<Allocator> & ByteBuffer<Allocator>::read(T& val) {
    return read(&val, sizeof(T));
}

//写入读出string
template<typename Allocator>
ByteBuffer<Allocator> & ByteBuffer<Allocator>::write(const std::string& val) {
    return write(val.data(), val.size());
}

template<typename Allocator>
ByteBuffer<Allocator> & ByteBuffer<Allocator>::read(std::string& val) {
    val.assign(in_, out_);
    clear();
    return *this;
}

//写入读出vector
template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::write(const std::vector<T>& val) {
    return write(reinterpret_cast<const char*>(val.data()), val.size() * sizeof(T));
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator> & ByteBuffer<Allocator>::read(std::vector<T>& val) {
    T* p = (T*)in_;
    std::size_t size = this->size() / sizeof(T);
    val.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        val[i] = p[i];
    }
    consume(size * sizeof(T));
    return *this;
}

//写入读出array
template<typename Allocator>
template<typename T, std::size_t N>
ByteBuffer<Allocator> & ByteBuffer<Allocator>::write(const std::array<T, N>& val) {
    return write(reinterpret_cast<const char*>(val.data()), N * sizeof(T));
}

template<typename Allocator>
template<typename T, std::size_t N>
ByteBuffer<Allocator> & ByteBuffer<Allocator>::read(std::array<T, N>& val) {
    std::size_t size = N * sizeof(T) <= this->size() ? N * sizeof(T) : this->size();
    T* p = static_cast<T*>(in_);
    for (size_t i = 0; i < size / sizeof(T); ++i) {
        val[i] = p[i];
    }
    consume(size / sizeof(T));
    return *this;
}

template<class Allocator>
template<typename T>
void ByteBuffer<Allocator>::copyFrom(ByteBuffer<T> const& other) {
    std::size_t const n = other.size();
    //n==0需要清楚数据
    //n>capacity()需要重新申请空间
    if(n == 0 || n > capacity()) {
        if(begin_ != nullptr) {
            allocator_.deallocate(begin_, this->capacity());
            begin_ = nullptr;
            in_ = nullptr;
            out_ = nullptr;
            last_ = nullptr;
            end_ = nullptr;
        }
        if(n == 0)
            return;
        begin_ = allocator_.allocate(n);
        in_ = begin_;
        out_ = begin_ + n;
        last_ = begin_ + n;
        end_ = begin_ + n;
    }
    in_ = begin_;
    out_ = begin_ + n;
    last_ = begin_ + n;
    if(begin_) {
        std::memcpy(begin_, other.in_, n);
    }
}

template<class Allocator>
void ByteBuffer<Allocator>::reserve(std::size_t n) {
    if(max_size_ < n)
        max_size_ = n;
    if(n > capacity())
        prepare(n - size());
    else
        prepare(n);
}

}//namespace zinx_asio
#endif /* BYTE_BUFFER_HPP */
