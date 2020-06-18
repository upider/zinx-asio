#ifndef DATAGRAM_SERVER_HPP
#define DATAGRAM_SERVER_HPP

#include <functional>
#include <boost/asio/ip/udp.hpp>

#include "iserver.hpp"
#include "connection_option.hpp"
#include "io_context_pool.hpp"

namespace zinx_asio {//namespace zinx_asio

class UDPMessage;
class MessageManager;

class DatagramServer: public IServer {
    public:
        DatagramServer(uint32_t ioWorkerPoolSize = 1,
                       uint32_t taskWorkerQueueNum = 0,
                       uint32_t taskWorkerPoolSize = 0,
                       const std::string& name = "",
                       const std::string& version = "",
                       uint32_t maxPackageSize = 512);
        virtual ~DatagramServer();

    public:
        //accept
        virtual void doAccept(size_t index);
        //start 开始
        virtual void start();
        //stop 停止
        virtual void stop();
        //server 运行
        virtual void serve();
        //addRouter 添加路由
        virtual void addRouter(uint32_t, const std::string&);
        //设置监听地址
        virtual void addEndPoint(const std::string&, int);
        //RouterMap 返回连接管理模块
        virtual std::map<uint32_t, std::string> getRouters();
        //getConnOptionMgr 返回连接管理模块
        std::shared_ptr<ConnOptions> getConnOptionMgr();

    private:
        //ioWorker池的worker数量
        uint32_t ioWorkerPoolSize_;
        //taskWorker池的worker数量
        uint32_t taskWorkerPoolSize_;
        uint32_t taskWorkerQueueNum_;
        //taskWorker池的worker数量
        //taskWorker池
        std::shared_ptr<io_context_pool> taskWorkerPool_;
        //io_context_pool
        std::unique_ptr<io_context_pool> ioWorkerPool_;
        //是否运行
        bool running_;
        //服务器名称
        std::string name_;
        //版本
        std::string version_;
        //最大数据包大小
        uint32_t maxPackageSize_;
        //router ID与名称
        std::map<uint32_t, std::string> routerMap_;
        std::shared_ptr<MessageManager> routers_ptr;
        //套接字选项管理模块
        std::shared_ptr<ConnOptions> connOption_ptr;
        //要连接的udp socket
        using socket = boost::asio::ip::udp::socket;
        std::vector<std::shared_ptr<socket>> sockets_;
        //多个监听端口
        std::vector<boost::asio::ip::udp::endpoint> endpoints_;
};

}//namespace zinx_asio
#endif /* DATAGRAM_SERVER_HPP */
