#include <memory>
#include <iostream>
#include <thread>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include "utils.hpp"
#include "server.hpp"
#include "data_pack.hpp"
#include "request.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio
Connection::Connection(Server* s, boost::asio::io_context& ioc, uint32_t id,
                       std::shared_ptr<ConnManager> cm, std::shared_ptr<MessageManager> mm)
    : belongServer_(s), socket_(ioc), connID_(id), isClosed_(false),
      connMgr_wptr(cm), routers_ptr(mm), strand_(ioc) {}

Connection::~Connection () {}

//startRead 读业务
void Connection::startRead(boost::asio::yield_context yield) {
    printf("[Connection %d ReadHandler Start]\n", connID_);

    //创建拆解包对象
    //读取客户端户message head的数据流的前八个字节
    size_t len = DataPack::getInstance().getHeadLen();
    readerBuffer_.prepare(len);
    try {
        boost::asio::async_read(socket_, readerBuffer_, boost::asio::transfer_exactly(len), yield);
    } catch(std::exception& ec) {
        std::cout << "[Writer exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        stop();
        return;
    }

    //拆包:读取messageID和Len放进headData
    Message msg = DataPack::getInstance().unpack(readerBuffer_);
    //清除buffer数据
    readerBuffer_.consume(len);
    //拆包:读取data
    len = msg.getMsgLen();
    readerBuffer_.prepare(len);
    try {
        boost::asio::async_read(socket_, readerBuffer_, boost::asio::transfer_exactly(len), yield);
    } catch(std::exception& ec) {
        std::cout << "[Writer exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        stop();
        return;
    }
    std::iostream ios(&readerBuffer_);
    char data[len];
    ios.read(data, len);
    msg.setData(data, len);
    //清空buffer数据
    readerBuffer_.consume(len);

    auto self(shared_from_this());

    //如果业务线程池存在，将任务放进线程池
    if (belongServer_->getTaskWorkerPool()) {
        boost::asio::post(belongServer_->getTaskWorkerPool()->getCtx(), [self, this, msg]() {
            //组装Request
            Request req(msg, self);
            routers_ptr->doMsgHandler(req);
            std::cout << "DoMsgHandler Thread ID " << std::this_thread::get_id() << std::endl;
            printf("[Connection %d ReadHandler Stop]\n", connID_);
            //head和data都发送
            //调用一次startWrite
            boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
                startWrite(yield);
            });
        });
    } else {
        //或直接执行
        //组装Request
        Request req(msg, self);
        routers_ptr->doMsgHandler(req);
        std::cout << "DoMsgHandler Thread ID " << std::this_thread::get_id() << std::endl;
        printf("[Connection %d ReadHandler Stop]\n", connID_);
        boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
            startWrite(yield);
        });
    }
}

//startWrite 写业务
void Connection::startWrite(boost::asio::yield_context yield) {
    printf("[Connection %d WriterHandler Start]\n", connID_);
    auto self(shared_from_this());

    try {
        boost::asio::async_write(socket_, writerBuffer_.data(), boost::asio::transfer_exactly(writerBuffer_.size()), yield);
    } catch(std::exception& ec) {
        std::cout << "[Writer exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        stop();
        return;
    }

    std::cout << "Writer Thread ID " << std::this_thread::get_id() << std::endl;

    //清除buffer数据
    writerBuffer_.consume(writerBuffer_.size());

    printf("[Connection %d WriterHandler Stop]\n", connID_);
    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
        startRead(yield);
    });
}

//start 启动链接
void Connection::start() {
    printf("==================No.%d Connection Start==================\n", connID_);
    auto self(shared_from_this());
    //调用OnConnStart
    belongServer_->callOnConnStart(self);

    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
        startRead(yield);
    });
}

//stop 停止链接
void Connection::stop() {
    printf("==================No.%d Connection Stop==================\n", connID_);
    //销毁连接之前调用OnConnStop
    auto self(shared_from_this());
    belongServer_->callOnConnStop(self);
    if(isClosed_) {
        return;
    }
    isClosed_ = true;
    socket_.cancel();
    socket_.close();
    auto p = connMgr_wptr.lock();
    if(p != nullptr) {
        p->delConn(self);
    } else {
        std::cout << "No." << connID_ << " Connection connMgr_wptr is nullptr" << std::endl;
    }
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, const char* data, size_t size) {
    DataPack::getInstance().pack(Message(msgID, data, size), writerBuffer_);
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, std::vector<char>& data) {
    DataPack::getInstance().pack(Message(msgID, data.data(), data.size()), writerBuffer_);
}

//getSocket 获取当前连接绑定的socket
boost::asio::ip::tcp::socket& Connection::getSocket() {
    return socket_;
}

//getConnID 获取当前连接的ID
uint32_t Connection::getConnID() const {
    return connID_;
}

}//namespace zinx_asio
