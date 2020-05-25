#ifndef ICONNECTION_HPP
#define ICONNECTION_HPP

#include <iostream>
#include <ext/pool_allocator.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/range.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/streambuf.hpp>

namespace zinx_asio {//namespace zinx_asio

template<typename T>
class ByteBuffer;
class IMessage;

class IConnection {
    public:
        IConnection() {}
        virtual ~IConnection () {}

        //更新timer截止时间
        virtual void setTimerCallback() = 0;
        //startRead&startWrite要被包装成协程
        //startRead 读业务
        virtual void startRead(boost::asio::yield_context yield) = 0;
        //start 启动链接
        virtual void start() = 0;
        //stop 停止链接
        virtual void stop() = 0;
        //getSocket 获取当前连接绑定的socket
        virtual boost::asio::ip::tcp::socket& getSocket() = 0;
        //getConnID 获取当前连接的ID
        virtual uint32_t getConnID() const = 0;
        //SendMsg 发送数据
        virtual void sendMsg(const char* data, size_t) = 0;
        //SendMsg 发送数据
        virtual void sendMsg(const std::vector<char>&) = 0;
        virtual void sendMsg(std::vector<char>&&) = 0;
        virtual void sendMsg(const std::vector<char>*) = 0;
        virtual void sendMsg(const std::shared_ptr<std::vector<char>>) = 0;
        //SendMsg 发送数据
        virtual void sendMsg(const std::string&) = 0;
        virtual void sendMsg(std::string&&) = 0;
        virtual void sendMsg(const std::string*) = 0;
        virtual void sendMsg(const std::shared_ptr<std::string>) = 0;
        //SendMsg 发送数据
        virtual void sendMsg(boost::asio::streambuf&) = 0;
        virtual void sendMsg(boost::asio::streambuf&&) = 0;
        virtual void sendMsg(boost::asio::streambuf*) = 0;
        virtual void sendMsg(std::shared_ptr<boost::asio::streambuf>) = 0;
        //SendMsg 发送数据
        virtual void sendMsg(IMessage&) = 0;
        virtual void sendMsg(IMessage&&) = 0;
        virtual void sendMsg(IMessage*) = 0;
        virtual void sendMsg(std::shared_ptr<IMessage>) = 0;
        //SendMsg 发送数据
        virtual void sendMsg(ByteBuffer<std::allocator<char>>& buffer) = 0;
        virtual void sendMsg(ByteBuffer<std::allocator<char>>* buffer) = 0;
        virtual void sendMsg(ByteBuffer<std::allocator<char>>&& buffer) = 0;
        virtual void sendMsg(std::shared_ptr<ByteBuffer<std::allocator<char>>> buffer) = 0;
        virtual void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>& buffer) = 0;
        virtual void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>* buffer) = 0;
        virtual void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>&& buffer) = 0;
        virtual void sendMsg(std::shared_ptr<ByteBuffer<__gnu_cxx::__pool_alloc<char>>> buffer) = 0;
};

}//namespace zinx_asio
#endif /* ICONNECTION_HPP */
