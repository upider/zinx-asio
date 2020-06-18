#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <functional>
#include <iostream>
#include <cassert>

#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "conn_manager.hpp"
#include "connection_option.hpp"
#include "data_pack.hpp"
#include "io_context_pool.hpp"
#include "iserver.hpp"
#include "message_manager.hpp"
#include "utils.hpp"

namespace zinx_asio {//namespace zinx_asio

class IConnection;
class ConnManager;

/*************ConnectionServer****************/

template<typename ConnectionType>
class ConnectionServer: public IServer {
    public:
        using Conn_ptr = typename std::shared_ptr<ConnectionType>;
        ConnectionServer(uint32_t ioWorkerPoolSize,
                         uint32_t taskWorkerQueueNum = 0,
                         uint32_t taskWorkerPoolSize = 0,
                         const std::string& name = "",
                         const std::string& version = "",
                         uint32_t maxPackageSize = 512,
                         uint32_t maxConnNum = 1024,
                         uint32_t maxConnIdleTime = 0);
        virtual ~ConnectionServer();

    public:
        //accept
        void doAccept(size_t);
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
        void setOnConnStart(std::function<void(Conn_ptr)>);
        //setOnConnStop 注册OnConnStop
        void setOnConnStop(std::function<void(Conn_ptr)>);
        //callOnConnStart 调用OnConnStart
        void callOnConnStart(Conn_ptr);
        //callOnConnStop 调用OnConnStop
        void callOnConnStop(Conn_ptr);
        //设置监听地址
        void addEndPoint(const std::string&, int);

        //设置acceptor
        template <typename SettableSocketOption >
        ConnectionServer& setAcceptorOption(const SettableSocketOption & option) {
            for (auto& acceptor : acceptors_) {
                acceptor->set_option(option);
            }
            return *this;
        }

        //getConnMgr 返回连接管理模块
        std::shared_ptr<ConnManager> getConnMgr();
        //getConnOptionMgr 返回连接管理模块
        std::shared_ptr<ConnOptions> getConnOptionMgr();
        //RouterMap 返回连接管理模块
        std::map<uint32_t, std::string> getRouters();
        //getTaskWorkerPool 返回连接管理模块
        std::shared_ptr<io_context_pool>& getTaskWorkerPool();

    private:
        //TODO:
        //给Connection设置套接字选项
        void setSocketOptions(std::shared_ptr<IConnection>);
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
        //最大数据包大小
        uint32_t maxPackageSize_;
        //router ID与名称
        std::map<uint32_t, std::string> routerMap_;
        //Connection
        Conn_ptr newConn_;
        //连接管理模块
        std::shared_ptr<ConnManager> connMgr_ptr;
        //套接字选项管理模块
        std::shared_ptr<ConnOptions> connOption_ptr;
        //Connection创建连接之后自动调用
        std::function<void(Conn_ptr)> onConnStart;
        //Connection销毁之前自动调用
        std::function<void(Conn_ptr)> onConnStop;
        //asio acceptor
        using acceptor = boost::asio::ip::tcp::acceptor;
        std::vector<std::unique_ptr<acceptor>> acceptors_;
        //连接id(序号)
        uint32_t cid_ = 0;
        //stopped
        std::atomic_bool stopped_{false};
        //多个监听端口
        std::list<boost::asio::ip::tcp::endpoint> endpoints_;
};

template<typename ConnectionType>
ConnectionServer<ConnectionType>::ConnectionServer(uint32_t ioWorkerPoolSize,
        uint32_t taskWorkerQueueNum, uint32_t taskWorkerPoolSize,
        const std::string& name, const std::string& version, uint32_t maxPackageSize,
        uint32_t maxConnNum, uint32_t maxConnIdleTime)
    : ioWorkerPoolSize_(ioWorkerPoolSize),
      taskWorkerPoolSize_(taskWorkerPoolSize),
      taskWorkerQueueNum_(taskWorkerQueueNum),
      ioWorkerPool_(new io_context_pool(ioWorkerPoolSize_, ioWorkerPoolSize_)),
      name_(name),
      version_(version),
      maxConnNum_(maxConnNum),
      maxConnIdleTime_(maxConnIdleTime),
      maxPackageSize_(maxPackageSize),
      connMgr_ptr(new ConnManager()),
      connOption_ptr(new ConnOptions()) {

    assert(taskWorkerPoolSize_ >= taskWorkerQueueNum_ && taskWorkerQueueNum_ >= 0);
    assert(ioWorkerPoolSize_ != 0);
    if (taskWorkerQueueNum_ > 0) {
        taskWorkerPool_.reset(new io_context_pool(taskWorkerPoolSize_, taskWorkerQueueNum_));
    }
}

template<typename ConnectionType>
ConnectionServer<ConnectionType>::~ConnectionServer() {
    ioWorkerPool_->stop();
}

//设置监听地址
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::addEndPoint(const std::string& host, int port) {
    endpoints_.emplace_back(boost::asio::ip::make_address(host), port);
}

template<typename ConnectionType>
void ConnectionServer<ConnectionType>::doAccept(size_t acceptorIndex) {
    //生成MessageManager
    auto msgMgr = std::make_shared<MessageManager>();
    for (auto r : routerMap_) {
        msgMgr->addRouter(r.first, CreateRouterObject(r.first));
    }

    //生成新的套接字
    auto newConn_ = std::make_shared<ConnectionType>(this, ioWorkerPool_->getCtx(),
                    cid_++, maxConnIdleTime_, maxPackageSize_,
                    connMgr_ptr, msgMgr);

    //开始等待连接
    std::cout << "Acceptor " << acceptorIndex << " start Accepting TCPConnection" << '\n';
    acceptors_[acceptorIndex]->async_accept(newConn_->getSocket(),
    [this, acceptorIndex, newConn_](boost::system::error_code ec) {
        if (ec) {
            std::cout << "ERROR: " << ec.message() << std::endl;
            acceptors_[acceptorIndex]->close();
            return;
        }
        //设置最大连接数
        if (connMgr_ptr->size() == maxConnNum_) {
            printf("---------------Excess MaxConnNum---------------\n");
            //TODO 给客户端一个错误响应
            newConn_->getSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            newConn_->getSocket().cancel();
            newConn_->getSocket().close();
        } else {
            connMgr_ptr->addConn(newConn_);
            newConn_->start();
            //给套接字设置套接字选项
            connOption_ptr->setConnOptions(newConn_);
        }
        if (stopped_.load(std::memory_order_relaxed)) {
            return;
        }
        doAccept(acceptorIndex);
    });
}

//start listen,初始化acceptors并真正开始listen
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::startListen() {
    //获取tcp的endpoints
    for (auto& e : GlobalObject::endPoints()) {
        endpoints_.emplace_back(boost::asio::ip::make_address(e.first), e.second);
    }
    //初始化acceptors
    for (size_t i = 0; i < endpoints_.size(); i++) {
        acceptors_.emplace_back(new acceptor(ioWorkerPool_->getCtx()));
    }
    size_t i = 0;
    //开启所有[地址:端口]的监听
    for (auto endpoint : endpoints_) {
        acceptors_[i]->open(endpoint.protocol());
        acceptors_[i]->bind(endpoint);
        acceptors_[i]->listen();
        std::cout << "[zinx] start listening on " << endpoint.address().to_string()
                  << ":" << endpoint.port() << std::endl;
        i++;
    }
}

//listen
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::listen(std::map<std::string, int> endpoints) {
    for (auto e : endpoints) {
        GlobalObject::addEndPoint(e.first, e.second);
    }
    startListen();
}

//listen
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::listen() {
    startListen();
}

//start 开始
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::start() {
    printf("[zinx] %s%s Start\n", name_.data(), version_.data());
    std::cout << "Max Connection num  = " << GlobalObject::maxConnNum() << std::endl;
    std::cout << "IOWorkerPoolSize    = " << ioWorkerPool_->iocNum() << std::endl;
    if (taskWorkerPool_ == nullptr) {
        std::cout << "TaskWorkerPool      = NULL" << std::endl;
    } else {
        std::cout << "TaskWorkerPoolSize  = " << taskWorkerPool_->threadNum() << std::endl;
        std::cout << "TaskWorkerQueueNum  = " << taskWorkerPool_->iocNum() << std::endl;
    }

    //开始执行acceptor
    for (size_t i = 0; i < acceptors_.size(); ++i) {
        doAccept(i);
    }
}

//server 运行
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::serve() {
    //添加signal
    boost::asio::signal_set signals(ioWorkerPool_->getCtx(), SIGINT, SIGTERM);
    signals.async_wait(
    [this](const boost::system::error_code & error, int signal_number) {
        if (error) {
            std::cout << "Signal Error: " << error.message() << '\n';
        }
        stop();
    });

    //启动acceptor
    start();

    //启动IO工作池
    ioWorkerPool_->run();
    std::cout << "[IOWorkerPool running]" << std::endl;
    //启动Task工作池
    if (taskWorkerPool_) {
        taskWorkerPool_->run();
        std::cout << "[TaskWorkerPool running]" << std::endl;
    }

    ioWorkerPool_->joinAll();
    if (taskWorkerPool_) {
        taskWorkerPool_->joinAll();
    }
}

//stop 停止
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::stop() {
    stopped_.store(true, std::memory_order_relaxed);
    //资源回收
    printf("[Zinx] %s:%s Start Stopping...\n", name_.data(), version_.data());

    if (taskWorkerPool_) {
        taskWorkerPool_->stop();
    }
    for (auto& i : acceptors_) {
        i->close();
    }
    ioWorkerPool_->stop();
    connMgr_ptr->stop();
    connMgr_ptr->clear();
    printf("[Zinx] %s:%s Stopped\n", name_.data(), version_.data());
}

//设置空闲连接时间
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::setMaxConnIdleTime(std::size_t time) {
    maxConnIdleTime_ = time;
}

//addRouter 添加路由
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::addRouter(uint32_t msgID, const std::string& name) {
    routerMap_[msgID] = name;
}

//setOnConnStart 注册OnConnStart
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::setOnConnStart(std::function<void(Conn_ptr)> hookFunc) {
    onConnStart = std::move(hookFunc);
}

//setOnConnStop 注册OnConnStop
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::setOnConnStop(std::function<void(Conn_ptr)> hookFunc) {
    onConnStop = std::move(hookFunc);
}

//callOnConnStart 调用OnConnStart
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::callOnConnStart(Conn_ptr conn) {
    try {
        onConnStart(conn);
    } catch(...) {
        throw;
    }
}

//callOnConnStop 调用OnConnStop
template<typename ConnectionType>
void ConnectionServer<ConnectionType>::callOnConnStop(Conn_ptr conn) {
    try {
        onConnStop(conn);
    } catch(...) {
        throw;
    }
}

//RouterMap 返回连接管理模块
template<typename ConnectionType>
std::map<uint32_t, std::string> ConnectionServer<ConnectionType>::getRouters() {
    return routerMap_;
}

//getConnMgr 返回连接管理模块
template<typename ConnectionType>
std::shared_ptr<ConnManager> ConnectionServer<ConnectionType>::getConnMgr() {
    return connMgr_ptr;
}

//getConnOptionMgr 返回连接管理模块
template<typename ConnectionType>
std::shared_ptr<ConnOptions> ConnectionServer<ConnectionType>::getConnOptionMgr() {
    return connOption_ptr;
}

//getTaskWorkerPool 返回连接管理模块
template<typename ConnectionType>
std::shared_ptr<io_context_pool>& ConnectionServer<ConnectionType>::getTaskWorkerPool() {
    return taskWorkerPool_;
}

/*************ConnectionServer****************/
}//namespace zinx_asio

#endif /* SERVER_HPP */
