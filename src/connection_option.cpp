#include <boost/asio/socket_base.hpp>
#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/ip/multicast.hpp>

#include "connection_option.hpp"
#include "iconnection.hpp"

namespace zinx_asio {//namespace zinx_asio

ConnOptions::ConnOptions() {}
ConnOptions::~ConnOptions() {}

//通用属性
ConnOptions& ConnOptions::setOption(const boost::asio::ip::v6_only& opt) {
    ConnOptions::option opt2;
    opt2.v6_only = opt;
    options_.emplace(V6Only, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::do_not_route& opt) {
    ConnOptions::option opt2;
    opt2.do_not_route = opt;
    options_.emplace(DoNotRoute, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::enable_connection_aborted& opt) {
    option opt2;
    opt2.enable_connection_aborted = opt;
    options_.emplace(EnableConnectionAborted, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::keep_alive& opt) {
    option opt2;
    opt2.keep_alive = opt;
    options_.emplace(KeepAlive, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::linger& opt) {
    option opt2;
    opt2.linger = opt;
    options_.emplace(Linger, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::receive_buffer_size& opt) {
    option opt2;
    opt2.receive_buffer_size = opt;
    options_.emplace(RecvBufferSize, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::send_buffer_size& opt) {
    option opt2;
    opt2.send_buffer_size = opt;
    options_.emplace(SendBufferSize, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::receive_low_watermark& opt) {
    option opt2;
    opt2.receive_low_watermark = opt;
    options_.emplace(RecvLowWatermark, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::send_low_watermark& opt) {
    option opt2;
    opt2.send_low_watermark = opt;
    options_.emplace(SendLowWatermark, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::reuse_address& opt) {
    option opt2;
    opt2.reuse_address = opt;
    options_.emplace(ReuseAddr, opt2);
    return *this;
}

//tcp属性
ConnOptions& ConnOptions::setOption(const boost::asio::ip::tcp::no_delay& opt) {
    option opt2;
    opt2.no_delay = opt;
    options_.emplace(NoDelay, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::debug& opt) {
    option opt2;
    opt2.debug = opt;
    options_.emplace(DeBug, opt2);
    return *this;
}

//udp属性
ConnOptions& ConnOptions::setOption(const boost::asio::socket_base::broadcast& opt) {
    option opt2;
    opt2.broadcast = opt;
    options_.emplace(Broadcast, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::ip::multicast::enable_loopback& opt) {
    option opt2;
    opt2.enable_loopback = opt;
    options_.emplace(EnableLoopBack, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::ip::multicast::hops& opt) {
    option opt2;
    opt2.multicast_hops = opt;
    options_.emplace(MultiHops, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::ip::multicast::join_group& opt) {
    option opt2;
    opt2.join_group = opt;
    options_.emplace(JoinGroup, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::ip::multicast::leave_group& opt) {
    option opt2;
    opt2.leave_group = opt;
    options_.emplace(LeaveGroup, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::ip::multicast::outbound_interface& opt) {
    option opt2;
    opt2.outbound_interface = opt;
    options_.emplace(OutboundInterface, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(const boost::asio::ip::unicast::hops& opt) {
    option opt2;
    opt2.unicast_hops = opt;
    options_.emplace(UniHops, opt2);
    return *this;
}

ConnOptions& ConnOptions::setOption(bool opt) {
    option opt2;
    opt2.non_block = opt;
    options_.emplace(NonBlock, opt2);
    return *this;
}

//给套接字设置套接字选项
void ConnOptions::setConnOptions(Conn_ptr conn) {
    for (auto& option : options_) {
        auto opt = option.second;
        switch (option.first) {
        case NonBlock: {
            conn->getSocket().non_blocking(opt.non_block);
            break;
        }
        case EnableLoopBack: {
            conn->getSocket().set_option(opt.enable_loopback);
            break;
        }
        case MultiHops: {
            conn->getSocket().set_option(opt.multicast_hops);
            break;
        }
        case JoinGroup: {
            conn->getSocket().set_option(opt.join_group);
            break;
        }
        case LeaveGroup: {
            conn->getSocket().set_option(opt.leave_group);
            break;
        }
        case OutboundInterface: {
            conn->getSocket().set_option(opt.outbound_interface);
            break;
        }
        case Broadcast: {
            conn->getSocket().set_option(opt.broadcast);
            break;
        }
        case UniHops: {
            conn->getSocket().set_option(opt.unicast_hops);
            break;
        }
        case DeBug: {
            conn->getSocket().set_option(opt.debug);
            break;
        }
        case DoNotRoute: {
            conn->getSocket().set_option(opt.do_not_route);
            break;
        }
        case EnableConnectionAborted: {
            conn->getSocket().set_option(opt.enable_connection_aborted);
            break;
        }
        case KeepAlive: {
            conn->getSocket().set_option(opt.keep_alive);
            break;
        }
        case Linger: {
            conn->getSocket().set_option(opt.linger);
            break;
        }
        case NoDelay: {
            conn->getSocket().set_option(opt.no_delay);
            break;
        }
        case RecvBufferSize: {
            conn->getSocket().set_option(opt.receive_buffer_size);
            break;
        }
        case SendBufferSize: {
            conn->getSocket().set_option(opt.send_buffer_size);
            break;
        }
        case SendLowWatermark: {
            conn->getSocket().set_option(opt.send_low_watermark);
            break;
        }
        case RecvLowWatermark: {
            conn->getSocket().set_option(opt.receive_low_watermark);
            break;
        }
        case ReuseAddr: {
            conn->getSocket().set_option(opt.reuse_address);
            break;
        }
        case V6Only: {
            conn->getSocket().set_option(opt.v6_only);
            break;
        }
        default:
            break;
        }
    }
}

}//namespace zinx_asio
