#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include "utils.hpp"
#include "server.hpp"
#include "data_pack.hpp"
#include "request.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio
Connection::Connection(Server* s, boost::asio::io_context& ioc, uint32_t id, size_t time,
                       std::shared_ptr<ConnManager> cm, std::shared_ptr<MessageManager> mm)
    : belongServer_(s), socket_(ioc), connID_(id), maxConnTime_(time), isClosed_(false),
      connMgr_wptr(cm), routers_ptr(mm), strand_(ioc), timer_(ioc) {}

Connection::~Connection () {}

//startRead 读业务
void Connection::startRead(boost::asio::yield_context yield) {
    printf("[Connection %d ReadHandler Start]\n", connID_);

    //创建拆解包对象
    //读取客户端户message head的数据流的前八个字节
    size_t len = DataPack().getHeadLen();
    auto msg = std::make_shared<Message>();

    char dataBuf[len];
    try {
        std::cout << "Server Read Data Head " << len << " bytes" << "\n";
        //boost::asio::async_read(socket_, msg->getData().buf(), boost::asio::transfer_exactly(len), yield);
        boost::asio::async_read(socket_, boost::asio::buffer(dataBuf, len),
                                boost::asio::transfer_exactly(len), yield);
    } catch(std::exception& ec) {
        std::cout << "[Writer exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        //终止timer
        timer_.cancel();
        stop();
        return;
    }

    //拆包:读取messageID和Len放进headData
    uint32_t id;
    len = dataBuf[0];
    id = dataBuf[4];
    //TODO:
    //msg->getData() >> len >> id;
    msg->setMsgLen(len);
    msg->setMsgID(id);
    try {
        std::cout << "Server Read Data Body: Body length is " << len << "\n";
        boost::asio::async_read(socket_, msg->getData().buf(),
                                boost::asio::transfer_exactly(len), yield);
    } catch(std::exception& ec) {
        std::cout << "[Writer exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        //终止timer
        timer_.cancel();
        stop();
        return;
    }

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
        boost::asio::async_write(socket_, writerBuffer_, boost::asio::transfer_all(), yield);
    } catch(std::exception& ec) {
        std::cout << "[Writer exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        //终止timer
        timer_.cancel();
        stop();
        return;
    }

    std::cout << "Writer Thread ID " << std::this_thread::get_id() << std::endl;

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

    if (maxConnTime_) {
        //设置timer超时时间
        timer_.expires_after(std::chrono::seconds(maxConnTime_));
        timer_.async_wait(
        [this](const boost::system::error_code & ec) {
            if (ec) {
                std::cout << "Connection " << connID_ << " Timer Error: " << ec.message() << std::endl;
                if(!isClosed_.load(std::memory_order_relaxed)) {
                    stop();
                    return;
                }
            } else {
                std::cout << "Connection " << connID_ << " Timeout" << std::endl;
                //取消Connection的读写操作,关闭Connection
                stop();
            }
        });
    }
}

//stop 停止链接
void Connection::stop() {
    printf("==================No.%d Connection Stop==================\n", connID_);
    //销毁连接之前调用OnConnStop
    auto self(shared_from_this());
    belongServer_->callOnConnStop(self);
    if(isClosed_.load(std::memory_order_relaxed)) {
        return;
    }
    isClosed_.store(true, std::memory_order_relaxed);
    socket_.cancel();
    socket_.close();
    auto p = connMgr_wptr.lock();
    if(p != nullptr) {
        p->delConn(self);
    } else {
        std::cout << "No." << connID_ << " Connection connMgr_wptr is nullptr" << std::endl;
    }
}

//TODO:startWrite放到sendMsg
//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, const char* data, size_t size) {
    std::ostream os(&writerBuffer_);
    //len
    os.write((char*)(&size), sizeof(size));
    //ID
    os.write((char*)(&msgID), sizeof(msgID));
    //数据
    os.write(data, size);
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, const std::vector<char>& data) {
    std::ostream os(&writerBuffer_);
    uint32_t size = data.size();
    //len
    os.write((char*)&size, sizeof(size));
    //ID
    os.write((char*)&msgID, sizeof(msgID));
    //数据
    os.write(data.data(), size);
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, const std::string& data) {
    std::ostream os(&writerBuffer_);
    uint32_t size = data.size();
    //len
    os << size;
    //os.write((char*)&size, sizeof(size));
    //ID
    os << msgID;
    //os.write((char*)&msgID, sizeof(msgID));
    //数据
    os << data;
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, boost::asio::streambuf& data) {
    std::ostream os(&writerBuffer_);
    //dataLen写进buf
    uint32_t len = data.size();
    os.write((char*)(&len), 4);
    //dataID写进buf
    os.write((char*)(&msgID), 4);
    //data写进buf
    os << &data;
}

//SendMsg 发送数据
void Connection::sendMsg(Message& msg) {
    std::ostream os(&writerBuffer_);
    //dataLen写进buf
    uint32_t len = msg.getMsgLen();
    os.write((char*)(&len), 4);
    //dataID写进buf
    uint32_t id = msg.getMsgID();
    os.write((char*)(&id), 4);
    //data写进buf
    os << msg.getData();
}

//getSocket 获取当前连接绑定的socket
boost::asio::ip::tcp::socket& Connection::getSocket() {
    return socket_;
}

//getConnID 获取当前连接的ID
uint32_t Connection::getConnID() const {
    return connID_;
}

//getRemoteEndpoint 获取客户端的TCP状态IP和Port
boost::asio::ip::tcp::endpoint Connection::getRemoteEndpoint() {
    return socket_.remote_endpoint();
}

//getLocalEndpoint 获取本地的TCP状态IP和Port
boost::asio::ip::tcp::endpoint Connection::getLocalEndpoint() {
    return socket_.local_endpoint();
}

}//namespace zinx_asio
