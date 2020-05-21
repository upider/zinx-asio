#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio/read.hpp>

#include "conn_manager.hpp"
#include "byte_buffer_stream.hpp"
#include "data_pack.hpp"
#include "request.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio

Connection::Connection(Server<boost::asio::ip::tcp>* s, boost::asio::io_context& ioc, uint32_t id, size_t time,
                       std::shared_ptr<ConnManager> cm, std::shared_ptr<MessageManager> mm)
    : belongServer_(s), socket_(ioc), connID_(id), maxConnIdleTime_(time), isClosed_(false),
      connMgr_wptr(cm), routers_ptr(mm), strand_(ioc), timer_(ioc) {}

Connection::~Connection () {}

//更新timer截止时间
void Connection::setTimerCallback() {
    auto self(shared_from_this());
    //设置timer超时时间
    timer_.async_wait(
    [this, self](const boost::system::error_code & ec) {
        if (ec) {
            //如果有错误，应该是timer被cancel
            std::cout << "Connection " << connID_ << " Timer Error: " << ec.message() << '\n';
        }
        //当前已经超时
        if(timer_.expiry() <= boost::asio::steady_timer::clock_type::now()) {
            timeOut_.store(true, std::memory_order_relaxed);
            std::cout << "Connection " << connID_ << " Timeout" << '\n';
            stop();
            return;
        } else {
            //未超时,连接未关闭
            if (!isClosed_.load(std::memory_order_relaxed)) {
                //重新设置timer
                setTimerCallback();
            }
        }
    });
}

//startRead 读业务
void Connection::startRead(boost::asio::yield_context yield) {
    printf("[Connection %d ReadHandler Start]\n", connID_);

    //创建拆解包对象
    //读取客户端户message head的数据流的前八个字节
    uint32_t len = DataPack().getHeadLen();
    uint32_t id{0};
    char head[len];

    try {
        boost::asio::async_read(socket_, boost::asio::buffer(head, len),
                                boost::asio::transfer_exactly(len), yield);
    } catch(std::exception& ec) {
        std::cout << "[Read head exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        stop();
        return;
    }

    //拆包:读取messageID和Len放进headData
    //解包错误,直接关闭客户端
    try {
        DataPack().unpack(len, id, head);
    } catch(const std::exception& e) {
        std::cout << "[Data Unpack Error] " << e.what() << '\n';
        //读取错误或终止时
        stop();
    }

    auto msg = std::make_shared<Message>(id, len);
    try {
        boost::asio::async_read(socket_, msg->getData().prepare(len),
                                boost::asio::transfer_exactly(len), yield);
        msg->getData().commit(len);
    } catch(std::exception& ec) {
        std::cout << "[Read body exits error] " << ec.what() << std::endl;
        //读取错误或终止时
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
            printf("[Connection %d ReadHandler Stop]\n", connID_);
        });
    } else {
        //或直接执行
        //组装Request
        Request req(msg, self);
        routers_ptr->doMsgHandler(req);
        printf("[Connection %d ReadHandler Stop]\n", connID_);
    }

    if (maxConnIdleTime_) {
        timer_.expires_after(std::chrono::seconds(maxConnIdleTime_));
    }
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

    if (maxConnIdleTime_) {
        //开始时设置timer存活时间
        timer_.expires_after(std::chrono::seconds(maxConnIdleTime_));
        setTimerCallback();
    }

    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
        startRead(yield);
    });
}

//stop 停止链接
void Connection::stop() {
    if(isClosed_.load(std::memory_order_relaxed)) {
        return;
    }
    printf("==================No.%d Connection Stop==================\n", connID_);
    //销毁连接之前调用OnConnStop
    auto self(shared_from_this());
    belongServer_->callOnConnStop(self);
    isClosed_.store(true, std::memory_order_relaxed);

    //不是time out导致stop
    if (!timeOut_.load(std::memory_order_relaxed)) {
        timer_.cancel();
    }
    //关闭socket
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
    auto self(shared_from_this());
    auto buffer = std::make_shared<ByteBufferStream<>>();
    buffer->writeUint32(size).writeUint32(msgID);
    buffer->write(data, size);
    boost::asio::spawn(strand_,
    [this, self, buffer](boost::asio::yield_context yield) {
        try {
            boost::asio::async_write(socket_, buffer->data(),
                                     boost::asio::transfer_all(), yield);
        } catch(std::exception& ec) {
            std::cout << "[sendMsg exits error] " << ec.what() << std::endl;
            //读取错误或终止时
            stop();
            return;
        }
    });
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, const std::vector<char>& data) {
    auto self(shared_from_this());
    auto buffer = std::make_shared<ByteBufferStream<>>();
    buffer->writeUint32(data.size()).writeUint32(msgID).write(data.data(), data.size());
    boost::asio::spawn(strand_,
    [this, self, buffer](boost::asio::yield_context yield) {
        try {
            boost::asio::async_write(socket_, buffer->data(),
                                     boost::asio::transfer_all(), yield);
        } catch(std::exception& ec) {
            std::cout << "[sendMsg exits error] " << ec.what() << std::endl;
            //读取错误或终止时
            stop();
            return;
        }
    });
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, const std::string& data) {
    auto self(shared_from_this());

    auto buffer = std::make_shared<ByteBufferStream<>>(DataPack().getHeadLen() + data.size());
    buffer->writeUint32(data.size()).writeUint32(msgID).writeString(data);

    boost::asio::spawn(strand_,
    [this, self, buffer](boost::asio::yield_context yield) {
        try {
            boost::asio::async_write(socket_, buffer->data(),
                                     boost::asio::transfer_all(), yield);
        } catch(std::exception& ec) {
            std::cout << "[sendMsg exits error] " << ec.what() << std::endl;
            //读取错误或终止时
            stop();
            return;
        }
    });
}

//SendMsg 发送数据
void Connection::sendMsg(uint32_t msgID, boost::asio::streambuf& data) {
    auto self(shared_from_this());
    auto buffer = std::make_shared<ByteBufferStream<>>();
    buffer->writeUint32(data.size()).writeUint32(msgID);
    buffer->operator<<(data);
    boost::asio::spawn(strand_,
    [this, self, buffer](boost::asio::yield_context yield) {
        try {
            boost::asio::async_write(socket_, buffer->data(),
                                     boost::asio::transfer_all(), yield);
        } catch(std::exception& ec) {
            std::cout << "[sendMsg exits error] " << ec.what() << std::endl;
            //读取错误或终止时
            stop();
            return;
        }
    });
}

//SendMsg 发送数据
void Connection::sendMsg(Message& msg) {
    auto self(shared_from_this());
    auto buffer = std::make_shared<ByteBufferStream<>>();
    buffer->writeUint32(msg.getMsgLen()).writeUint32(msg.getMsgID());
    buffer->operator<<(msg.getData());
    boost::asio::spawn(strand_,
    [this, self, buffer](boost::asio::yield_context yield) {
        try {
            boost::asio::async_write(socket_, buffer->data(),
                                     boost::asio::transfer_all(), yield);
        } catch(std::exception& ec) {
            std::cout << "[sendMsg exits error] " << ec.what() << std::endl;
            //读取错误或终止时
            stop();
            return;
        }
    });
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
