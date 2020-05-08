#include <iostream>

#include <boost/asio/socket_base.hpp>
#include <boost/asio/ip/v6_only.hpp>

#include "conn_manager.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio
ConnManager::ConnManager() {}
ConnManager::~ConnManager() {}

//add 添加连接
void ConnManager::addConn(Conn_ptr conn) {
    connsLock.lock();
    conns[conn->getConnID()] = conn;
    connsLock.unlock();
    printf("==============%d Connection added to ConnManager, conn num = %lu==============\n", conn->getConnID(), conns.size());
}

//del 删除链接
void ConnManager::delConn(Conn_ptr conn) {
    connsLock.lock();
    conns.erase(conn->getConnID());
    connsLock.unlock();
    printf("============%d Connection removed from ConnManager, conn num = %lu==============\n", conn->getConnID(), conns.size());
}

//get 根据ID获取连接
Conn_ptr ConnManager::getConn(uint32_t id) {
    connsLock.lock_shared();
    auto it = conns.find(id);
    connsLock.unlock_shared();
    if (it != conns.end()) {
        return it->second;
    }
    return nullptr;
}

//size 获取当前连接总数
uint32_t ConnManager::size() {
    connsLock.lock_shared();
    size_t s = conns.size();
    connsLock.unlock_shared();
    return s;
}

//clear 清除所有连接
void ConnManager::clear() {
    connsLock.lock();
    for (auto it = conns.begin(); it != conns.end(); ++it) {
        it->second->stop();
    }
    conns.clear();
    connsLock.unlock();
}

//设置非阻塞
void ConnManager::setNonBlock(Conn_ptr conn, bool nonBlock) {
    conn->getSocket().non_blocking(nonBlock);
}

//设置接收缓冲区大小
void ConnManager::setRecvBufferSize(Conn_ptr conn, size_t nSize) {
    boost::asio::socket_base::receive_buffer_size size_option(nSize);
    conn->getSocket().set_option(size_option);
}

//设置发送缓冲区大小
void ConnManager::setSendBufferSize(Conn_ptr conn, size_t nSize) {
    boost::asio::socket_base::send_buffer_size size_option(nSize);
    conn->getSocket().set_option(size_option);
}

//设置端口复用
void ConnManager::setReuseAddr(Conn_ptr conn, bool reuse) {
    conn->getSocket().set_option(boost::asio::ip::tcp::acceptor::reuse_address(reuse));
}

//设置no_delay
void ConnManager::setNoDelay(Conn_ptr conn, bool noDelay) {
    conn->getSocket().set_option(boost::asio::ip::tcp::no_delay(noDelay));
}


//设置只能使用ipv6
void ConnManager::setV6Only(Conn_ptr conn, bool v6) {
    conn->getSocket().set_option(boost::asio::ip::v6_only(v6));
}

//设置广播
void ConnManager::setBroadcast(Conn_ptr conn, bool broadcast) {
    conn->getSocket().set_option(boost::asio::socket_base::broadcast(broadcast));
}

//设置debug
void ConnManager::setDebug(Conn_ptr conn, bool debug) {
    conn->getSocket().set_option(boost::asio::socket_base::debug(debug));
}

//设置do not route
void ConnManager::setDoNotRoute(Conn_ptr conn, bool doNotRoute) {
    conn->getSocket().set_option(boost::asio::socket_base::do_not_route(doNotRoute));
}

//设置keep alive
void ConnManager::setKeepAlive(Conn_ptr conn, bool keepAlive) {
    conn->getSocket().set_option(boost::asio::socket_base::keep_alive(keepAlive));
}

//设置延迟关闭的时间
void ConnManager::setLinger(Conn_ptr conn, bool linger, size_t time) {
    conn->getSocket().set_option(boost::asio::socket_base::linger(linger, time));
}

//设置receive_low_watermark
void ConnManager::setRecvLowWaterMark(Conn_ptr conn, size_t mark) {
    conn->getSocket().set_option(boost::asio::socket_base::receive_low_watermark(mark));
}

//设置send_low_watermark
void ConnManager::setSendLowWaterMark(Conn_ptr conn, size_t mark) {
    conn->getSocket().set_option(boost::asio::socket_base::send_low_watermark(mark));
}

//添加套接字选项:如果添加的是liger选项那么默认添加为真
ConnManager& ConnManager::addSocketOption(SocketOption option, int val) {
    options_[option] = val;
    return *this;
}

//NonBlock,
//SendBufferSize,
//RecvBufferSize,
//ReuseAddr,
//NoDelay,
//V6Only,
//Broadcast,
//DeBug,
//DoNotRoute,
//KeepAlive,
//Linger,
//RecvLowWatermark,
//SendLowWatermark,

//给套接字设置套接字选项
void ConnManager::setAllSocketOptions(Conn_ptr conn) {
    for (auto& option : options_) {
        switch (option.first) {
        case NonBlock: {
            setNonBlock(conn, option.second);
            break;
        }
        case SendBufferSize:
        {
            setSendBufferSize(conn, option.second);
            break;
        }
        case RecvBufferSize: {
            setRecvBufferSize(conn, option.second);
            break;
        }
        case ReuseAddr: {
            setReuseAddr(conn, option.second);
            break;
        }
        case NoDelay: {
            setNoDelay(conn, option.second);
            break;
        }
        case V6Only: {
            setV6Only(conn, option.second);
            break;
        }
        case Broadcast: {
            setBroadcast(conn, option.second);
            break;
        }
        case DeBug: {
            setDebug(conn, option.second);
            break;
        }
        case DoNotRoute: {
            setDoNotRoute(conn, option.second);
            break;
        }
        case KeepAlive: {
            setKeepAlive(conn, option.second);
            break;
        }
        case Linger: {
            setLinger(conn, true, option.second);
            break;
        }
        case RecvLowWatermark: {
            setRecvBufferSize(conn, option.second);
            break;
        }
        case SendLowWatermark: {
            setSendBufferSize(conn, option.second);
            break;
        }
        default: {
            std::cout << "No Such SocketOption" << std::endl;
            break;
        }
        }
    }
}

}//namespace zinx_asio
