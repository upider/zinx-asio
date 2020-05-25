#include <boost/asio/socket_base.hpp>
#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/ip/multicast.hpp>

#include "connection_option.hpp"
#include "iconnection.hpp"

namespace zinx_asio {//namespace zinx_asio

ConnectionOption::ConnectionOption() {}
ConnectionOption::~ConnectionOption() {}

//设置非阻塞
void ConnectionOption::setNonBlock(Conn_ptr conn, bool nonBlock) {
    conn->getSocket().non_blocking(nonBlock);
}

//设置接收缓冲区大小
void ConnectionOption::setRecvBufferSize(Conn_ptr conn, size_t nSize) {
    boost::asio::socket_base::receive_buffer_size size_option(nSize);
    conn->getSocket().set_option(size_option);
}

//设置发送缓冲区大小
void ConnectionOption::setSendBufferSize(Conn_ptr conn, size_t nSize) {
    boost::asio::socket_base::send_buffer_size size_option(nSize);
    conn->getSocket().set_option(size_option);
}

//设置端口复用
void ConnectionOption::setReuseAddr(Conn_ptr conn, bool reuse) {
    conn->getSocket().set_option(boost::asio::ip::tcp::acceptor::reuse_address(reuse));
}

//设置no_delay
void ConnectionOption::setNoDelay(Conn_ptr conn, bool noDelay) {
    conn->getSocket().set_option(boost::asio::ip::tcp::no_delay(noDelay));
}


//设置只能使用ipv6
void ConnectionOption::setV6Only(Conn_ptr conn, bool v6) {
    conn->getSocket().set_option(boost::asio::ip::v6_only(v6));
}

//设置广播
void ConnectionOption::setBroadcast(Conn_ptr conn, bool broadcast) {
    conn->getSocket().set_option(boost::asio::socket_base::broadcast(broadcast));
}

//设置debug
void ConnectionOption::setDebug(Conn_ptr conn, bool debug) {
    conn->getSocket().set_option(boost::asio::socket_base::debug(debug));
}

//设置do not route
void ConnectionOption::setDoNotRoute(Conn_ptr conn, bool doNotRoute) {
    conn->getSocket().set_option(boost::asio::socket_base::do_not_route(doNotRoute));
}

//设置keep alive
void ConnectionOption::setKeepAlive(Conn_ptr conn, bool keepAlive) {
    conn->getSocket().set_option(boost::asio::socket_base::keep_alive(keepAlive));
}

//设置延迟关闭的时间
void ConnectionOption::setLinger(Conn_ptr conn, bool linger, size_t time) {
    conn->getSocket().set_option(boost::asio::socket_base::linger(linger, time));
}

//设置receive_low_watermark
void ConnectionOption::setRecvLowWaterMark(Conn_ptr conn, size_t mark) {
    conn->getSocket().set_option(boost::asio::socket_base::receive_low_watermark(mark));
}

//设置send_low_watermark
void ConnectionOption::setSendLowWaterMark(Conn_ptr conn, size_t mark) {
    conn->getSocket().set_option(boost::asio::socket_base::send_low_watermark(mark));
}

//多播属性
void ConnectionOption::setMulticastEnableLoopBack(Conn_ptr conn, bool loopback) {
    conn->getSocket().set_option(boost::asio::ip::multicast::enable_loopback(loopback));
}

void ConnectionOption::setMulticastHops(Conn_ptr conn, int hops) {
    conn->getSocket().set_option(boost::asio::ip::multicast::hops(hops));
}

void ConnectionOption::setMulticastJoinGroup(Conn_ptr conn, const std::string& group) {
    boost::asio::ip::address multicast_address =
        boost::asio::ip::address::from_string(group);
    boost::asio::ip::multicast::join_group option(multicast_address);
    conn->getSocket().set_option(option);
}

void ConnectionOption::setMulticastLeaveGroup(Conn_ptr conn, const std::string& group) {
    boost::asio::ip::address multicast_address =
        boost::asio::ip::address::from_string(group);
    boost::asio::ip::multicast::leave_group option(multicast_address);
    conn->getSocket().set_option(option);
}

void ConnectionOption::setMulticastOutboundInterface(Conn_ptr conn, const std::string& group) {
    boost::asio::ip::address_v4 local_interface =
        boost::asio::ip::address_v4::from_string(group);
    boost::asio::ip::multicast::outbound_interface option(local_interface);
    conn->getSocket().set_option(option);
}

//添加套接字选项:如果添加的是liger选项那么默认添加为真
ConnectionOption& ConnectionOption::addSocketOption(SocketOption option, int val) {
    options_[option] = val;
    return *this;
}

//添加多播的套接字选项
ConnectionOption& ConnectionOption::addMulticastSocketOption(SocketOption option, int val) {
    options_[option] = val;
    return *this;
}

//添加多播的套接字选项
ConnectionOption& ConnectionOption::addMulticastSocketOption(SocketOption option, const std::string& val) {
    strOptions_[option] = val;
    return *this;
}

//M_EnableLoopBack,
//M_Hops,
//M_JoinGroup,
//M_LeaveGroup,
//M_OutboundInterface,
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
void ConnectionOption::setAllSocketOptions(Conn_ptr conn) {
    for(auto& option : strOptions_) {
        switch (option.first) {
        case M_JoinGroup: {
            setMulticastJoinGroup(conn, option.second);
            break;
        }
        case M_LeaveGroup: {
            setMulticastLeaveGroup(conn, option.second);
            break;
        }
        case M_OutboundInterface: {
            setMulticastOutboundInterface(conn, option.second);
            break;
        }
        default: {
            std::cout << "No Such SocketOption" << '\n';
            break;
        }
        }
    }
    for (auto& option : options_) {
        switch (option.first) {
        case M_EnableLoopBack: {
            setMulticastEnableLoopBack(conn, option.second);
            break;
        }
        case M_Hops: {
            setMulticastHops(conn, option.second);
            break;
        }
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
            std::cout << "No Such SocketOption" << '\n';
            break;
        }
        }
    }
}

}//namespace zinx_asio
