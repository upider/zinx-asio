#include <iostream>
#include <cassert>

#include <boost/asio/ip/address.hpp>

#include "conn_manager.hpp"
#include "connection.hpp"
#include "utils.hpp"
#include "server.hpp"

namespace zinx_asio {//namespace zinx_asio

Server<boost::asio::ip::tcp>::Server(const std::string& conf) {
    //得到配置文件的参数
    GlobalObject::parseConf(conf);
    //初始化
    name_ = GlobalObject::serverName();
    version_ = GlobalObject::zinxVersion();
    maxConnNum_ = GlobalObject::maxConnNum();
    maxConnIdleTime_ = GlobalObject::maxConnIdleTime();
    ioWorkerPoolSize_ = GlobalObject::ioWorkerPoolSize();
    taskWorkerPoolSize_ = GlobalObject::taskWorkerPoolSize();
    taskWorkerQueueNum_ = GlobalObject::taskWorkerQueueNum();
    //获取tcp的endpoints
    for (auto& e : GlobalObject::endPoints()) {
        endpoints_.emplace_back(boost::asio::ip::make_address(e.first), e.second);
    }
    ioWorkerPool_.reset(new io_context_pool(ioWorkerPoolSize_, ioWorkerPoolSize_));
    connMgr_ptr.reset(new ConnManager());

    assert(taskWorkerPoolSize_ >= taskWorkerQueueNum_ && taskWorkerQueueNum_ >= 0);
    assert(ioWorkerPoolSize_ != 0);
    if (taskWorkerQueueNum_ > 0) {
        taskWorkerPool_.reset(new io_context_pool(taskWorkerPoolSize_, taskWorkerQueueNum_));
    }
}

Server<boost::asio::ip::tcp>::Server(uint32_t ioWorkerPoolSize,
                                     uint32_t taskWorkerQueueNum,
                                     uint32_t taskWorkerPoolSize,
                                     const std::string& name,
                                     uint32_t maxPackageSize,
                                     uint32_t maxConnNum,
                                     uint32_t maxConnIdleTime)
    : ioWorkerPoolSize_(ioWorkerPoolSize),
      taskWorkerPoolSize_(taskWorkerPoolSize),
      taskWorkerQueueNum_(taskWorkerQueueNum),
      ioWorkerPool_(new io_context_pool(ioWorkerPoolSize_, ioWorkerPoolSize_)),
      name_(name),
      connMgr_ptr(new ConnManager()) {

    GlobalObject::ioWorkerPoolSize(ioWorkerPoolSize);
    GlobalObject::taskWorkerPoolSize(taskWorkerPoolSize);
    GlobalObject::taskWorkerQueueNum(taskWorkerQueueNum);
    GlobalObject::maxConnNum(maxConnNum);
    GlobalObject::maxPackageSize(maxPackageSize);
    GlobalObject::maxConnIdleTime(maxConnIdleTime);

    assert(taskWorkerPoolSize_ >= taskWorkerQueueNum_ && taskWorkerQueueNum_ >= 0);
    assert(ioWorkerPoolSize_ != 0);
    if (taskWorkerQueueNum_ > 0) {
        taskWorkerPool_.reset(new io_context_pool(taskWorkerPoolSize_, taskWorkerQueueNum_));
    }
}

Server<boost::asio::ip::tcp>::~Server() {
    ioWorkerPool_->stop();
}

//设置监听地址
void Server<boost::asio::ip::tcp>::addEndPoint(const std::string& host, int port) {
    GlobalObject::addEndPoint(host, port);
}

void Server<boost::asio::ip::tcp>::doAccept(size_t acceptorIndex) {
    //生成MessageManager
    auto msgMgr = std::make_shared<MessageManager>();
    for (auto r : routerMap_) {
        msgMgr->addRouter(r.first, CreateRouterObject(r.first));
    }

    //生成新的套接字
    auto newConn = std::make_shared<Connection>(this, ioWorkerPool_->getCtx(),
                   cid_++, maxConnIdleTime_,
                   connMgr_ptr, msgMgr);

    //开始等待连接
    std::cout << "Acceptor " << acceptorIndex << " start Accepting Connection" << std::endl;
    acceptors_[acceptorIndex]->async_accept(newConn->getSocket(),
    [this, acceptorIndex, newConn](boost::system::error_code ec) {
        if (ec) {
            std::cout << "ERROR: " << ec.message() << std::endl;
            return;
        }
        //设置最大连接数
        if (connMgr_ptr->size() == maxConnNum_) {
            printf("---------------Excess MaxConnNum---------------\n");
            //TODO 给客户端一个错误响应
            newConn->getSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            newConn->getSocket().cancel();
            newConn->getSocket().close();
        } else {
            connMgr_ptr->addConn(newConn);
            newConn->start();
            //给套接字设置套接字选项
            connMgr_ptr->setAllSocketOptions(newConn);
        }
        doAccept(acceptorIndex);
    });
}

//start listen,初始化acceptors并真正开始listen
void Server<boost::asio::ip::tcp>::startListen() {
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
void Server<boost::asio::ip::tcp>::listen(std::map<std::string, int> endpoints) {
    for (auto e : endpoints) {
        GlobalObject::addEndPoint(e.first, e.second);
    }
    startListen();
}

//listen
void Server<boost::asio::ip::tcp>::listen() {
    startListen();
}

//start 开始
void Server<boost::asio::ip::tcp>::start() {
    printf("[zinx] %s%s Start\n",
           name_.data(), version_.data());
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

//stop 停止
void Server<boost::asio::ip::tcp>::stop() {
    //资源回收
    printf("[zinx] has been stopped\n");
    if (taskWorkerPool_->threadNum() > 0) {
        taskWorkerPool_->joinAll();
    }
    if (taskWorkerPool_->threadNum() > 0) {
        taskWorkerPool_->stop();
    }
    ioWorkerPool_->stop();
    connMgr_ptr->clear();
}

//server 运行
void Server<boost::asio::ip::tcp>::serve() {
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
}


//设置空闲连接时间
void Server<boost::asio::ip::tcp>::setMaxConnIdleTime(std::size_t time) {
    GlobalObject::maxConnIdleTime(time);
    maxConnIdleTime_ = time;
}

//addRouter 添加路由
void Server<boost::asio::ip::tcp>::addRouter(uint32_t msgID, const std::string& name) {
    routerMap_[msgID] = name;
}

//setOnConnStart 注册OnConnStart
void Server<boost::asio::ip::tcp>::setOnConnStart(std::function<void(Conn_ptr)> hookFunc) {
    onConnStart = std::move(hookFunc);
}

//setOnConnStop 注册OnConnStop
void Server<boost::asio::ip::tcp>::setOnConnStop(std::function<void(Conn_ptr)> hookFunc) {
    onConnStop = std::move(hookFunc);
}

//callOnConnStart 调用OnConnStart
void Server<boost::asio::ip::tcp>::callOnConnStart(Conn_ptr conn) {
    try {
        onConnStart(conn);
    } catch(...) {
        throw;
    }
}

//callOnConnStop 调用OnConnStop
void Server<boost::asio::ip::tcp>::callOnConnStop(Conn_ptr conn) {
    try {
        onConnStop(conn);
    } catch(...) {
        throw;
    }
}

//getConnMgr 返回连接管理模块
std::shared_ptr<ConnManager> Server<boost::asio::ip::tcp>::getConnMgr() {
    return connMgr_ptr;
}

//getTaskWorkerPool 返回连接管理模块
std::shared_ptr<io_context_pool>& Server<boost::asio::ip::tcp>::getTaskWorkerPool() {
    return taskWorkerPool_;
}

}//namespace zinx_asio
