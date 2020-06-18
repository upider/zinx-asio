#ifndef CONNECTIO_OPTION_HPP
#define CONNECTIO_OPTION_HPP

#include <map>
#include <memory>

#include <boost/asio/socket_base.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/ip/unicast.hpp>
#include <boost/asio/ip/udp.hpp>

namespace zinx_asio {//namespace zinx_asio

class IConnection;

class ConnOptions {
    public:
        using Conn_ptr = typename std::shared_ptr<IConnection>;
        ConnOptions();
        ~ConnOptions();

        //通用属性
        ConnOptions& setOption(const boost::asio::ip::v6_only&);
        ConnOptions& setOption(const boost::asio::socket_base::do_not_route&);
        ConnOptions& setOption(const boost::asio::socket_base::enable_connection_aborted&);
        ConnOptions& setOption(const boost::asio::socket_base::keep_alive&);
        ConnOptions& setOption(const boost::asio::socket_base::linger&);
        ConnOptions& setOption(const boost::asio::socket_base::receive_buffer_size&);
        ConnOptions& setOption(const boost::asio::socket_base::send_buffer_size&);
        ConnOptions& setOption(const boost::asio::socket_base::receive_low_watermark&);
        ConnOptions& setOption(const boost::asio::socket_base::send_low_watermark&);
        ConnOptions& setOption(const boost::asio::socket_base::reuse_address&);
        //tcp属性
        ConnOptions& setOption(const boost::asio::ip::tcp::no_delay&);
        ConnOptions& setOption(const boost::asio::socket_base::debug&);
        //udp属性
        ConnOptions& setOption(const boost::asio::socket_base::broadcast&);
        ConnOptions& setOption(const boost::asio::ip::multicast::enable_loopback&);
        ConnOptions& setOption(const boost::asio::ip::multicast::hops&);
        ConnOptions& setOption(const boost::asio::ip::multicast::join_group&);
        ConnOptions& setOption(const boost::asio::ip::multicast::leave_group&);
        ConnOptions& setOption(const boost::asio::ip::multicast::outbound_interface&);
        ConnOptions& setOption(const boost::asio::ip::unicast::hops&);
        ConnOptions& setOption(bool NonBlock);

        //给套接字设置套接字选项
        void setConnOptions(Conn_ptr conn);
        std::map<int, union option> getConnOptions();

    private:
        union option {
            option() {}
            ~option() {}
            //用于确定IPv6套接字是否仅支持IPv6通信的套接字选项
            boost::asio::ip::v6_only v6_only;
            //套接字选项可防止路由，仅使用本地接口
            boost::asio::socket_base::do_not_route do_not_route;
            //套接字选项在接受时报告中止的连接
            boost::asio::socket_base::enable_connection_aborted enable_connection_aborted;
            //套接字选项发送保持活动状态。
            boost::asio::socket_base::keep_alive keep_alive;
            //套接字选项，用于指定如果存在未发送的数据，则套接字是否在关闭时徘徊
            boost::asio::socket_base::linger linger;
            //用于套接字的接收缓冲区大小的套接字选项
            boost::asio::socket_base::receive_buffer_size receive_buffer_size;
            //套接字的发送缓冲区大小的套接字选项
            boost::asio::socket_base::send_buffer_size send_buffer_size;
            //接收低水位的套接字选项
            boost::asio::socket_base::receive_low_watermark receive_low_watermark;
            //发送低水印的套接字选项
            boost::asio::socket_base::send_low_watermark send_low_watermark;
            //套接字选项，允许将套接字绑定到已经使用的地址
            boost::asio::socket_base::reuse_address reuse_address;
            //套接字选项，用于禁用Nagle算法
            boost::asio::ip::tcp::no_delay no_delay;
            //套接字选项可启用套接字级别的调试
            boost::asio::socket_base::debug debug;
            //套接字选项允许发送广播消息
            boost::asio::socket_base::broadcast broadcast;
            //套接字选项确定出站组播数据包是多播组的成员时,是否在同一套接字上接收到
            boost::asio::ip::multicast::enable_loopback enable_loopback;
            //与传出多播数据包关联的生存时间的套接字选项
            boost::asio::ip::multicast::hops multicast_hops;
            //用于在指定接口上加入多播组的套接字选项。
            boost::asio::ip::multicast::join_group join_group;
            //套接字选项可将多播组保留在指定接口上
            boost::asio::ip::multicast::leave_group leave_group;
            //本地接口的套接字选项可用于传出的多播数据包
            boost::asio::ip::multicast::outbound_interface outbound_interface;
            //与传出单播数据包关联的生存时间的套接字选项
            boost::asio::ip::unicast::hops unicast_hops;
            //非阻塞
            bool non_block;
        };

        /*! \enum SocketOption
         *
         *  Socket 的设置选项,写成枚举方便调用
         */
        enum SocketOption {
            //多播选项
            EnableLoopBack,
            MultiHops,
            JoinGroup,
            LeaveGroup,
            OutboundInterface,
            Broadcast,
            UniHops,
            //socket属性
            DeBug,
            DoNotRoute,
            EnableConnectionAborted,
            KeepAlive,
            Linger,
            NoDelay,
            NonBlock,
            RecvBufferSize,
            SendBufferSize,
            SendLowWatermark,
            RecvLowWatermark,
            ReuseAddr,
            V6Only,
        };

        //socket属性
        std::map<SocketOption, option> options_;
};

}//namespace zinx_asio

#endif /* CONNECTIO_OPTION_HPP */
