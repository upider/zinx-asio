#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/any.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/range.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>

#include "message_manager.hpp"

namespace zinx_asio {//namespace zinx_asio

class ConnManager;
class Server;

class Connection: public std::enable_shared_from_this<Connection> {
    public:
        Connection(Server*, boost::asio::io_context&, uint32_t,
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
        void sendMsg(uint32_t msgID, std::vector<char>&);
        //TODO
        //getRemoteAddr 获取客户端的TCP状态IP和Port
        //void getRemoteAddr();
        //SetProp 设置连接属性
        //void setProp(std::string key, boost::any val);
        //GetProp 获取连接属性
        //boost::any getProp(std::string key);
        //DelProp 删除连接属性
        //void delProp(std::string key);
    private:
        //当前connection所属的server
        Server* belongServer_;
        //获取当前连接绑定的socket
        boost::asio::ip::tcp::socket socket_;
        //当前连接ID
        uint32_t connID_;
        //当前连接状态
        bool isClosed_;
        //TODO:改成读写各一个缓冲
        //读写协程的数据缓冲
        boost::asio::streambuf readerBuffer_;
        //读写协程的数据缓冲
        boost::asio::streambuf writerBuffer_;
        //当前connection所属的connManager
        //防止循环引用
        std::weak_ptr<ConnManager> connMgr_wptr;
        //消息管理模块
        std::shared_ptr<MessageManager> routers_ptr;
        //TODO:
        //连接属性集合
        //std::map<std::string, boost::any> props_;
        //保护连接属性的锁
        //boost::shared_mutex propsLock_;
        //保证异步执行顺序
        boost::asio::io_context::strand strand_;
};

}//namespace zinx_asio
#endif /* CONNECTION_HPP */
