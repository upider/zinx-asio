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
        //accept
        void doAccept(size_t acceptorIndex);
        //start 开始
        void start();
        //listen
        void listen();
        //stop 停止
        void stop();
        //server 运行
        void serve();
        //addRouter 添加路由
        void addRouter(uint32_t, const std::string&);
        //setOnConnStart 注册OnConnStart
        void setOnConnStart(std::function<void(Conn_ptr)>);
        //setOnConnStop 注册OnConnStop
        void setOnConnStop(std::function<void(Conn_ptr)>);
        //callOnConnStart 调用OnConnStart
        void callOnConnStart(Conn_ptr);
        //callOnConnStop 调用OnConnStop
        void callOnConnStop(Conn_ptr);

        //设置acceptor
        template <typename SettableSocketOption >
        Server& setAcceptorOption(const SettableSocketOption & option) {
            for (auto& acceptor : acceptors_) {
                acceptor->set_option(option);
            }
            return *this;
        }

        //getConnMgr 返回连接管理模块
        std::shared_ptr<ConnManager> getConnMgr();
        //RouterMap 返回连接管理模块
        std::map<uint32_t, std::string> getRouters();
        //getTaskWorkerPool 返回连接管理模块
        std::shared_ptr<io_context_pool>& getTaskWorkerPool();

    private:
        //TODO:
        //给Connection设置套接字选项
        void setSocketOptions(Conn_ptr);

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
        //router ID与名称
        std::map<uint32_t, std::string> routerMap_;
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
};

}//namespace zinx_asio
#endif /* SERVER_HPP */
