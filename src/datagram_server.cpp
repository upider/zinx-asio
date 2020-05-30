#include <cassert>

#include "datagram.hpp"
#include "datagram_server.hpp"
#include "message_manager.hpp"
#include "udp_message.hpp"

namespace zinx_asio {//namespace zinx_asio

DatagramServer::DatagramServer(uint32_t ioWorkerPoolSize, uint32_t taskWorkerQueueNum,
                               uint32_t taskWorkerPoolSize, const std::string& name,
                               uint32_t maxPackageSize)
    : ioWorkerPoolSize_(ioWorkerPoolSize),
      taskWorkerPoolSize_(taskWorkerPoolSize),
      taskWorkerQueueNum_(taskWorkerQueueNum),
      ioWorkerPool_(new io_context_pool(ioWorkerPoolSize_, ioWorkerPoolSize_)),
      name_(name),
      maxPackageSize_(maxPackageSize),
      routers_ptr(new MessageManager()),
      connOption_ptr(new ConnectionOption()) {

    assert(taskWorkerPoolSize_ >= taskWorkerQueueNum_ && taskWorkerQueueNum_ >= 0);
    assert(ioWorkerPoolSize_ != 0);
    if (taskWorkerQueueNum_ > 0) {
        taskWorkerPool_.reset(new io_context_pool(taskWorkerPoolSize_, taskWorkerQueueNum_));
    }
}

DatagramServer::~DatagramServer() {}

//accept
void DatagramServer::doAccept(size_t index) {
    auto msg = std::make_shared<UDPMessage>(maxPackageSize_);
    //客户端的地址
    auto dataGram = std::make_shared<Datagram>(sockets_[index]);
    sockets_[index]->async_receive_from(msg->bufferRef().prepare(maxPackageSize_),
                                        dataGram->getEndPoint(),
    [this, dataGram, index, msg](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "Read Error: " << ec.message() << '\n';
            stop();
        }
        if(length > maxPackageSize_) {
            std::cout << "Read Error: excess max package size " << '\n';
        }

        msg->commit(length);
        //如果业务线程池存在，将任务放进线程池
        if (taskWorkerPool_) {
            boost::asio::post(taskWorkerPool_->getCtx(),
            [this, dataGram, msg]() {
                routers_ptr->doMsgHandler(dataGram, msg);
            });
        } else {
            //或直接执行
            routers_ptr->doMsgHandler(dataGram, msg);
        }

        doAccept(index);
    });
}

//start 开始
void DatagramServer::start() {
    printf("[zinx] %s%s Start\n", name_.data(), version_.data());
    std::cout << "IOWorkerPoolSize    = " << ioWorkerPool_->iocNum() << '\n';
    if (taskWorkerPool_ == nullptr) {
        std::cout << "TaskWorkerPool      = NULL" << '\n';
    } else {
        std::cout << "TaskWorkerPoolSize  = " << taskWorkerPool_->threadNum() << '\n';
        std::cout << "TaskWorkerQueueNum  = " << taskWorkerPool_->iocNum() << '\n';
    }

    //生成MessageManager
    for (auto r : routerMap_) {
        routers_ptr->addRouter(r.first, CreateRouterObject(r.first));
    }

    for (std::size_t i = 0; i < endpoints_.size(); ++i) {
        sockets_.emplace_back(new boost::asio::ip::udp::socket(ioWorkerPool_->getCtx(),
                              endpoints_[i]));
    }

    //开始执行doaccept
    for (size_t i = 0; i < sockets_.size(); ++i) {
        doAccept(i);
    }
}

//stop 停止
void DatagramServer::stop() {
    //资源回收
    for (auto i : sockets_) {
        i->cancel();
        i->close();
    }
    printf("[zinx] has been stopped\n");
    if (taskWorkerPool_->threadNum() > 0) {
        taskWorkerPool_->joinAll();
    }
    if (taskWorkerPool_->threadNum() > 0) {
        taskWorkerPool_->stop();
    }
    ioWorkerPool_->stop();
}

//server 运行
void DatagramServer::serve() {
    //启动acceptor
    start();

    //启动IO工作池
    ioWorkerPool_->run();
    std::cout << "[IOWorkerPool running]" << '\n';
    //启动Task工作池
    if (taskWorkerPool_) {
        taskWorkerPool_->run();
        std::cout << "[TaskWorkerPool running]" << '\n';
    }

    ioWorkerPool_->joinAll();
}

//addRouter 添加路由
void DatagramServer::addRouter(uint32_t id, const std::string& name) {
    routerMap_[id] = name;
}

//设置监听地址
void DatagramServer::addEndPoint(const std::string& host, int port) {
    endpoints_.emplace_back(boost::asio::ip::make_address(host), port);
}

//RouterMap 返回连接管理模块
std::map<uint32_t, std::string> DatagramServer::getRouters() {
    return routerMap_;
}

//getConnOptionMgr 返回连接管理模块
std::shared_ptr<ConnectionOption> DatagramServer::getConnOptionMgr() {
    return connOption_ptr;
}

}//namespace zinx_asio
