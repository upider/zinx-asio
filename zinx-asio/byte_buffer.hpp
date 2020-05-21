#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP

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
                BOOST_THROW_EXCEPTION(std::length_error{
                "basic_flat_buffer too long"});
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
            if(n >= distance(in_, out_)) {
                in_ = begin_;
                out_ = begin_;
                return;
            }
            in_ += n;
        }
        //dynamic_buffer_v1 requirements

        std::pair<char*, std::size_t> getRawBuffer();
        std::vector<char> toVector();
        std::string tosString();
        void clear()noexcept;
        void reserve(std::size_t n);

        /* TODO:  <20-05-20, yourname> */
        ByteBuffer& write(void* val, std::size_t size);
        ByteBuffer& read(char* val, std::size_t size);

    private:
        void copyFrom(ByteBuffer const& other);
        static std::size_t distance(char const* first, char const* last)noexcept;

    private:
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
ByteBuffer<Allocator>::ByteBuffer(const ByteBuffer<T>& other)
    : max_size_(other.max_size_) {
    copyFrom(other);
}

template<typename Allocator>
template<typename T>
ByteBuffer<Allocator>::ByteBuffer(ByteBuffer<T>&& other)
    : max_size_(other.max_size_) {
    copyFrom(other);
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
    max_size_ = other.max_size_;
    copyFrom(other);
    return *this;
}

template<typename Allocator>
ByteBuffer<Allocator>::~ByteBuffer() {
    if (begin_) {
        allocator_.deallocate(begin_, this->capacity());
        begin_ = nullptr;
    }
}

template<typename Allocator>
std::pair<char*, std::size_t> ByteBuffer<Allocator>::getRawBuffer() {
    return {in_, size()};
}

template<typename Allocator>
std::vector<char> ByteBuffer<Allocator>::toVector() {
    return std::vector<char>(in_, out_);
}

template<typename Allocator>
std::string ByteBuffer<Allocator>::tosString() {
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

template<class Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::write(void* val, std::size_t size) {
    reserve(size);
    std::memcpy(out_, (char*)val, size);
    commit(size);
    return *this;
}

template<class Allocator>
ByteBuffer<Allocator>& ByteBuffer<Allocator>::read(char* val, std::size_t size) {
    size = size < this->size() ? size : this->size();
    std::memcpy(val, in_, size);
    consume(size);
    return *this;
}

template<class Allocator>
void ByteBuffer<Allocator>::copyFrom(ByteBuffer const& other) {
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
}

}//namespace zinx_asio
#endif /* BYTE_BUFFER_HPP */
