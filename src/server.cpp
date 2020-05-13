#include <iostream>
#include <cassert>

#include "connection.hpp"
#include "utils.hpp"
#include "server.hpp"

namespace zinx_asio {//namespace zinx_asio

Server::Server()
    : ioWorkerPoolSize_(GlobalObject::getInstance().IOWorkerPoolSize),
      taskWorkerPoolSize_(GlobalObject::getInstance().TaskWorkerPoolSize),
      taskWorkerQueueNum_(GlobalObject::getInstance().TaskWorkerQueueNum),
      ioWorkerPool_(new io_context_pool(ioWorkerPoolSize_, ioWorkerPoolSize_)),
      name_(GlobalObject::getInstance().Name),
      connMgr_ptr(new ConnManager()) {

    assert(taskWorkerPoolSize_ >= taskWorkerQueueNum_ && taskWorkerQueueNum_ >= 0);
    assert(ioWorkerPoolSize_ != 0);
    if (taskWorkerQueueNum_ > 0) {
        taskWorkerPool_.reset(new io_context_pool(taskWorkerPoolSize_, taskWorkerQueueNum_));
    }

    //获取tcp的endpoints
    endpoints_ = GlobalObject::getInstance().EndPoints;
    acceptors_.reserve(endpoints_.size());
    for (size_t i = 0; i < endpoints_.size(); i++) {
        acceptors_.emplace_back(new acceptor(ioWorkerPool_->getCtx()));
    }
}

Server::~Server() {
    ioWorkerPool_->stop();
}

void Server::doAccept(size_t acceptorIndex) {
    //生成MessageManager
    auto msgMgr = std::make_shared<MessageManager>();
    for (auto r : routerMap_) {
        msgMgr->addRouter(r.first, CreateRouterObject(r.first));
    }

    //生成新的套接字
    auto newConn = std::make_shared<Connection>(this, ioWorkerPool_->getCtx(),
                   cid_++, GlobalObject::getInstance().MaxConnTime,
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
        if (connMgr_ptr->size() == GlobalObject::getInstance().MaxConn) {
            printf("---------------Excess MaxConn---------------\n");
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

//listen
void Server::listen() {
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

//start 开始
void Server::start() {
    printf("[zinx] %s%s Start\n",
           GlobalObject::getInstance().Name.data(), GlobalObject::getInstance().ZinxVersion.data());
    std::cout << "Max Connection num  = " << GlobalObject::getInstance().MaxConn << std::endl;
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
void Server::stop() {
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
void Server::serve() {
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

//addRouter 添加路由
void Server::addRouter(uint32_t msgID, const std::string& name) {
    routerMap_[msgID] = name;
}

//setOnConnStart 注册OnConnStart
void Server::setOnConnStart(std::function<void(Conn_ptr)> hookFunc) {
    onConnStart = std::move(hookFunc);
}

//setOnConnStop 注册OnConnStop
void Server::setOnConnStop(std::function<void(Conn_ptr)> hookFunc) {
    onConnStop = std::move(hookFunc);
}

//callOnConnStart 调用OnConnStart
void Server::callOnConnStart(Conn_ptr conn) {
    try {
        onConnStart(conn);
    } catch(...) {
        throw;
    }
}

//callOnConnStop 调用OnConnStop
void Server::callOnConnStop(Conn_ptr conn) {
    try {
        onConnStop(conn);
    } catch(...) {
        throw;
    }
}

//getConnMgr 返回连接管理模块
std::shared_ptr<ConnManager> Server::getConnMgr() {
    return connMgr_ptr;
}

//getTaskWorkerPool 返回连接管理模块
std::shared_ptr<io_context_pool>& Server::getTaskWorkerPool() {
    return taskWorkerPool_;
}

}//namespace zinx_asio
