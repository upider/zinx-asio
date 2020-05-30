#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP

#include <iostream>
#include <atomic>

#include <boost/thread/shared_mutex.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>

#include "iconnection.hpp"
#include "message_manager.hpp"
#include "data_pack.hpp"

namespace zinx_asio {//namespace zinx_asio

template<typename T>
class ConnectionServer;

class IMessage;
class ConnManager;

class TCPConnection:
    public std::enable_shared_from_this<TCPConnection>,
    virtual public IConnection {
    public:
        TCPConnection(ConnectionServer<TCPConnection>*, boost::asio::io_context&,
                      uint32_t, size_t, uint32_t,
                      std::shared_ptr<ConnManager>, std::shared_ptr<MessageManager>);
        virtual ~TCPConnection ();
        //更新timer截止时间
        void setTimerCallback();
        //startRead&startWrite要被包装成协程
        //startRead 读业务
        void startRead(boost::asio::yield_context yield);
        //start 启动链接
        void start();
        //stop 停止链接
        void stop();
        //getSocket 获取当前连接绑定的socket
        boost::asio::ip::tcp::socket& getSocket();
        //getConnID 获取当前连接的ID
        uint32_t getConnID() const;
        //SendMsg 发送数据
        virtual void sendMsg(const char* data, size_t);
        //SendMsg 发送数据
        virtual void sendMsg(const std::vector<char>&);
        virtual void sendMsg(std::vector<char>&&);
        virtual void sendMsg(const std::vector<char>*);
        virtual void sendMsg(const std::shared_ptr<std::vector<char>>);
        //SendMsg 发送数据
        virtual void sendMsg(const std::string&);
        virtual void sendMsg(std::string&&);
        virtual void sendMsg(const std::string*);
        virtual void sendMsg(const std::shared_ptr<std::string>);
        //SendMsg 发送数据
        virtual void sendMsg(boost::asio::streambuf&);
        virtual void sendMsg(boost::asio::streambuf&&);
        virtual void sendMsg(boost::asio::streambuf*);
        virtual void sendMsg(std::shared_ptr<boost::asio::streambuf>);
        //SendMsg 发送数据
        virtual void sendMsg(IMessage&);
        virtual void sendMsg(IMessage&&);
        virtual void sendMsg(IMessage*);
        virtual void sendMsg(std::shared_ptr<IMessage>);
        //SendMsg 发送数据
        virtual void sendMsg(ByteBuffer<std::allocator<char>>& buffer);
        virtual void sendMsg(ByteBuffer<std::allocator<char>>* buffer);
        virtual void sendMsg(ByteBuffer<std::allocator<char>>&& buffer);
        virtual void sendMsg(std::shared_ptr<ByteBuffer<std::allocator<char>>> buffer);
        virtual void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>& buffer);
        virtual void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>* buffer);
        virtual void sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>&& buffer);
        virtual void sendMsg(std::shared_ptr<ByteBuffer<__gnu_cxx::__pool_alloc<char>>> buffer);

        //getRemoteEndpoint 获取客户端的TCP状态IP和Port
        boost::asio::ip::tcp::endpoint getRemoteEndpoint();
        //getLocalEndpoint 获取本地的TCP状态IP和Port
        boost::asio::ip::tcp::endpoint getLocalEndpoint ();
    private:
        //当前connection所属的server
        ConnectionServer<TCPConnection>* belongServer_;
        //获取当前连接绑定的socket
        boost::asio::ip::tcp::socket socket_;
        //当前连接ID
        uint32_t connID_;
        //最大空闲连接时间
        size_t maxConnIdleTime_;
        //当前连接状态
        std::atomic_bool isClosed_;
        //是否超时
        std::atomic_bool timeOut_{false};
        //最大数据包大小
        DataPack dataPack_;
        //当前connection所属的connManager
        //防止循环引用
        std::weak_ptr<ConnManager> connMgr_wptr;
        //消息管理模块
        std::shared_ptr<MessageManager> routers_ptr;
        //保证异步执行顺序,封装协程
        boost::asio::io_context::strand strand_;
        //Connection 连接时间定时
        boost::asio::steady_timer timer_;
};

}//namespace zinx_asio
#endif /* TCPCONNECTION_HPP */
