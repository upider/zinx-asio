#ifndef DATAGRAM_HPP
#define DATAGRAM_HPP

#include <ext/pool_allocator.h>
#include <boost/asio/ip/udp.hpp>
#include <boost/range.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/streambuf.hpp>

namespace zinx_asio {//namespace zinx_asio

template<typename T>
class ByteBuffer;
class IMessage;

class Datagram {
    public:
        Datagram(std::shared_ptr<boost::asio::ip::udp::socket>,
                 boost::asio::ip::udp::endpoint);
        Datagram(std::shared_ptr<boost::asio::ip::udp::socket>);
        ~Datagram();

        void setEndPoint(boost::asio::ip::udp::endpoint);
        boost::asio::ip::udp::endpoint& getEndPoint();

        //getSocket 获取当前连接绑定的socket
        std::shared_ptr<boost::asio::ip::udp::socket> getSocket();
        //SendMsg 发送数据
        void sendMsg(const char* data, size_t);
        //SendMsg 发送数据
        void sendMsg(const std::vector<char>&);
        void sendMsg(std::vector<char>&&);
        void sendMsg(const std::vector<char>*);
        void sendMsg(const std::shared_ptr<std::vector<char>>);
        //SendMsg 发送数据
        void sendMsg(const std::string&);
        void sendMsg(std::string&&);
        void sendMsg(const std::string*);
        void sendMsg(const std::shared_ptr<std::string>);
        //SendMsg 发送数据
        void sendMsg(boost::asio::streambuf&);
        void sendMsg(boost::asio::streambuf&&);
        void sendMsg(boost::asio::streambuf*);
        void sendMsg(std::shared_ptr<boost::asio::streambuf>);
        //SendMsg 发送数据
        void sendMsg(IMessage&);
        void sendMsg(IMessage&&);
        void sendMsg(IMessage*);
        void sendMsg(std::shared_ptr<IMessage>);
        //SendMsg 发送数据
        void sendMsg(ByteBuffer<std::allocator<char>>& buffer);
        void sendMsg(ByteBuffer<std::allocator<char>>* buffer);
        void sendMsg(ByteBuffer<std::allocator<char>>&& buffer);
        void sendMsg(std::shared_ptr<ByteBuffer<std::allocator<char>>> buffer);
        void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>& buffer);
        void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>* buffer);
        void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>&& buffer);
        void sendMsg(std::shared_ptr<ByteBuffer<__gnu_cxx::__pool_alloc<char>>> buffer);

    private:
        std::shared_ptr<boost::asio::ip::udp::socket> socket_;
        boost::asio::ip::udp::endpoint sendPoint_;
};

}//namespace zinx_asio
#endif /* DATAGRAM_HPP */
