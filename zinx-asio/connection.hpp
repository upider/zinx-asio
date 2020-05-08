#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <atomic>

#include <boost/thread/shared_mutex.hpp>
#include <boost/range.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

#include "message_manager.hpp"

namespace zinx_asio {//namespace zinx_asio

class ConnManager;
class Server;

class Connection: public std::enable_shared_from_this<Connection> {
    public:
        Connection(Server*, boost::asio::io_context&, uint32_t, size_t,
                   std::shared_ptr<ConnManager>, std::shared_ptr<MessageManager>);
        virtual ~Connection ();
        //startRead&startWrite要被包装成协程
        //startRead 读业务
        void startRead(boost::asio::yield_context yield);
        //startWrite 写业务
        void startWrite(boost::asio::yield_context yield);
        //start 启动链接
        void start();
        //stop 停止链接
        void stop();
        //getSocket 获取当前连接绑定的socket
        boost::asio::ip::tcp::socket& getSocket();
        //getConnID 获取当前连接的ID
        uint32_t getConnID() const;
        //SendMsg 发送数据
        void sendMsg(uint32_t msgID, const char* data, size_t);
        //SendMsg 发送数据
        void sendMsg(uint32_t msgID, const std::vector<char>&);
        //SendMsg 发送数据
        void sendMsg(uint32_t msgID, const std::string&);
        //SendMsg 发送数据
        void sendMsg(uint32_t msgID, boost::asio::streambuf&);
        //SendMsg 发送数据
        void sendMsg(const Message&);
        //getRemoteEndpoint 获取客户端的TCP状态IP和Port
        boost::asio::ip::tcp::endpoint getRemoteEndpoint();
        //getLocalEndpoint 获取本地的TCP状态IP和Port
        boost::asio::ip::tcp::endpoint getLocalEndpoint ();
    private:
        //当前connection所属的server
        Server* belongServer_;
        //获取当前连接绑定的socket
        boost::asio::ip::tcp::socket socket_;
        //当前连接ID
        uint32_t connID_;
        //最大连接时间
        size_t maxConnTime_;
        //当前连接状态
		std::atomic_bool isClosed_;
        //读写协程的数据缓冲
        boost::asio::streambuf readerBuffer_;
        //读写协程的数据缓冲
        boost::asio::streambuf writerBuffer_;
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
#endif /* CONNECTION_HPP */
