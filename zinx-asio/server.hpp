#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <functional>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "io_context_pool.hpp"
#include "conn_manager.hpp"
#include "message_manager.hpp"

namespace zinx_asio {//namespace zinx_asio

class Server {
    public:
        Server();
        virtual ~Server ();

    public:
        //getConnMgr 返回连接管理模块
        std::shared_ptr<ConnManager> getConnMgr();
        //getConnMgr 返回连接管理模块
        std::shared_ptr<MessageManager> getRouters();
        //getTaskWorkerPool 返回连接管理模块
        std::shared_ptr<io_context_pool>& getTaskWorkerPool();
        //accept
        void doAccept(size_t acceptorIndex);
        //start 开始
        void start();
        //stop 停止
        void stop();
        //server 运行
        void serve();
        //addRouter 添加路由
        void addRouter(uint32_t, std::shared_ptr<Router>);
        //setOnConnStart 注册OnConnStart
        void setOnConnStart(std::function<void(Conn_ptr)>);
        //setOnConnStop 注册OnConnStop
        void setOnConnStop(std::function<void(Conn_ptr)>);
        //callOnConnStart 调用OnConnStart
        void callOnConnStart(Conn_ptr);
        //callOnConnStop 调用OnConnStop
        void callOnConnStop(Conn_ptr);
    private:
        //ioWorker池的worker数量
        uint32_t ioWorkerPoolSize_;
        //taskWorker池的worker数量
        uint32_t taskWorkerPoolSize_;
        //taskWorker池的worker数量
        uint32_t taskWorkerQueueNum_;
        //taskWorker池
        std::shared_ptr<io_context_pool> taskWorkerPool_;
        //io_context_pool
        std::unique_ptr<io_context_pool> ioWorkerPool_;
        //是否运行
        bool running_;
        //服务器名称
        std::string name_;
        //ip版本
        std::string IPVersion_;
        //消息管理模块
        std::shared_ptr<MessageManager> routers_ptr;
        //连接管理模块
        std::shared_ptr<ConnManager> connMgr_ptr;
        //Connection创建连接之后自动调用
        std::function<void(Conn_ptr)> onConnStart;
        //Connection销毁之前自动调用
        std::function<void(Conn_ptr)> onConnStop;
        //asio acceptor
        using acceptor = boost::asio::ip::tcp::acceptor;
        std::vector<std::unique_ptr<acceptor>> acceptors_;
        //连接id(序号)
        uint32_t cid_ = 0;
        //多个监听端口
        std::list<boost::asio::ip::tcp::endpoint> endpoints_;
        //socket属性

};

}//namespace zinx_asio
#endif /* SERVER_HPP */
