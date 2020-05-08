#ifndef CONN_MANAGER_HPP
#define CONN_MANAGER_HPP

#include <atomic>
#include <map>
#include <memory>
#include <boost/thread/shared_mutex.hpp>

namespace zinx_asio {//namespace zinx_asio

/*! \enum SocketOption
 *
 *  Socket 的设置选项,写成枚举方便调用
 */
enum SocketOption {
    NonBlock,
    SendBufferSize,
    RecvBufferSize,
    ReuseAddr,
    NoDelay,
    V6Only,
    Broadcast,
    DeBug,
    DoNotRoute,
    KeepAlive,
    Linger,
    RecvLowWatermark,
    SendLowWatermark,
};

class Connection;
typedef std::shared_ptr<Connection> Conn_ptr;

class ConnManager {
    public:
        ConnManager();
        virtual ~ConnManager ();
        //add 添加连接
        void addConn(Conn_ptr);
        //del 删除链接
        void delConn(Conn_ptr);
        //get 根据ID获取连接
        Conn_ptr getConn(uint32_t);
        //size 获取当前连接总数
        uint32_t size();
        //clear 清除所有连接
        void clear();

        /*设置连接属性*/
        //设置非阻塞
        void setNonBlock(Conn_ptr conn, bool nonBlock);
        //设置发送缓冲区大小
        void setSendBufferSize(Conn_ptr conn, size_t nSize);
        //设置接收缓冲区大小
        void setRecvBufferSize(Conn_ptr conn, size_t nSize);
        //设置端口复用
        void setReuseAddr(Conn_ptr conn, bool reuse);
        //设置no_delay
        void setNoDelay(Conn_ptr conn, bool noDelay);
        //设置只能使用ipv6
        void setV6Only(Conn_ptr conn, bool v6);
        //设置广播
        void setBroadcast(Conn_ptr conn, bool broadcast);
        //设置debug
        void setDebug(Conn_ptr conn, bool debug);
        //设置do not route
        void setDoNotRoute(Conn_ptr conn, bool debug);
        //设置keep alive
        void setKeepAlive(Conn_ptr conn, bool keepAlive);
        //设置延迟关闭的时间
        void setLinger(Conn_ptr conn, bool linger, size_t time);
        //设置receive_low_watermark
        void setRecvLowWaterMark(Conn_ptr conn, size_t mark);
        //设置send_low_watermark
        void setSendLowWaterMark(Conn_ptr conn, size_t mark);
        //添加套接字选项:如果添加的是liger选项那么默认添加为真
        ConnManager& addSocketOption(SocketOption option, int val);
        //给套接字设置套接字选项
        void setAllSocketOptions(Conn_ptr conn);
    private:
        //socket属性
        std::map<SocketOption, int> options_;
        //连接集合
        std::map<uint32_t, Conn_ptr> conns;
        //连接集合的保护锁
        boost::shared_mutex connsLock;
};

}//namespace zinx_asio
#endif /* CONN_MANAGER_HPP */
