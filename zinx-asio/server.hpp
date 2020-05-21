#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <functional>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

#include "io_context_pool.hpp"
#include "message_manager.hpp"

namespace zinx_asio {//namespace zinx_asio

class ConnManager;
class Connection;

template<typename T>
class Server {};

template<> class Server<boost::asio::ip::tcp> {
    public:
        Server(const std::string&);
        Server(uint32_t ioWorkerPoolSize,
               uint32_t taskWorkerQueueNum = 0,
               uint32_t taskWorkerPoolSize = 0,
               const std::string& name = "",
               uint32_t maxPackageSize = 512,
               uint32_t maxConnNum = 1024,
               uint32_t maxConnIdleTime = 0);
        ~Server();

    public:
        //accept
        void doAccept(size_t acceptorIndex);
        //start 开始
        void start();
        //listen
        void listen();
        //listen
        void listen(std::map<std::string, int> endpoints);
        //stop 停止
        void stop();
        //server 运行
        void serve();
        //设置空闲连接时间
        void setMaxConnIdleTime(std::size_t);
        //addRouter 添加路由
        void addRouter(uint32_t, const std::string&);
        //setOnConnStart 注册OnConnStart
        void setOnConnStart(std::function<void(std::shared_ptr<Connection>)>);
        //setOnConnStop 注册OnConnStop
        void setOnConnStop(std::function<void(std::shared_ptr<Connection>)>);
        //callOnConnStart 调用OnConnStart
        void callOnConnStart(std::shared_ptr<Connection>);
        //callOnConnStop 调用OnConnStop
        void callOnConnStop(std::shared_ptr<Connection>);
        //设置监听地址
        void addEndPoint(const std::string&, int);

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
        void setSocketOptions(std::shared_ptr<Connection>);
        //start listen,初始化acceptors并真正开始listen
        void startListen();

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
        //最大连接数
        uint32_t maxConnNum_;
        //连接最大空闲时间
        uint32_t maxConnIdleTime_;
        //router ID与名称
        std::map<uint32_t, std::string> routerMap_;
        //连接管理模块
        std::shared_ptr<ConnManager> connMgr_ptr;
        //Connection创建连接之后自动调用
        std::function<void(std::shared_ptr<Connection>)> onConnStart;
        //Connection销毁之前自动调用
        std::function<void(std::shared_ptr<Connection>)> onConnStop;
        //asio acceptor
        using acceptor = boost::asio::ip::tcp::acceptor;
        std::vector<std::unique_ptr<acceptor>> acceptors_;
        //连接id(序号)
        uint32_t cid_ = 0;
        //多个监听端口
        std::list<boost::asio::ip::tcp::endpoint> endpoints_;
};

template<> class Server<boost::asio::ip::udp> {
    public:
        Server(const std::string&);
        Server(uint32_t ioWorkerPoolSize,
               uint32_t taskWorkerQueueNum = 0,
               uint32_t taskWorkerPoolSize = 0,
               const std::string& name = "",
               uint32_t maxPackageSize = 512,
               uint32_t maxConnNum = 1024,
               uint32_t maxConnIdleTime = 0);
        ~Server();

    public:
        //accept
        void doAccept(size_t acceptorIndex);
        //start 开始
        void start();
        //listen
        void listen();
        //listen
        void listen(std::map<std::string, int> endpoints);
        //stop 停止
        void stop();
        //server 运行
        void serve();
        //addRouter 添加路由
        void addRouter(uint32_t, const std::string&);
        //setOnConnStart 注册OnConnStart
        void setOnConnStart(std::function<void(std::shared_ptr<Connection>)>);
        //setOnConnStop 注册OnConnStop
        void setOnConnStop(std::function<void(std::shared_ptr<Connection>)>);
        //callOnConnStart 调用OnConnStart
        void callOnConnStart(std::shared_ptr<Connection>);
        //callOnConnStop 调用OnConnStop
        void callOnConnStop(std::shared_ptr<Connection>);
        //设置监听地址
        void addEndPoint(const std::string&, int);

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
        void setSocketOptions(std::shared_ptr<Connection>);
        //start listen,初始化acceptors并真正开始listen
        void startListen();

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
        //最大连接数
        uint32_t maxConnNum_;
        //连接最大空闲时间
        uint32_t maxConnIdleTime_;
        //router ID与名称
        std::map<uint32_t, std::string> routerMap_;
        //连接管理模块
        std::shared_ptr<ConnManager> connMgr_ptr;
        //Connection创建连接之后自动调用
        std::function<void(std::shared_ptr<Connection>)> onConnStart;
        //Connection销毁之前自动调用
        std::function<void(std::shared_ptr<Connection>)> onConnStop;
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
