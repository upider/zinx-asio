#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

#include <boost/asio/read.hpp>

#include "tcp_message.hpp"
#include "conn_manager.hpp"
#include "tcp_connection.hpp"
#include "connection_server.hpp"

namespace zinx_asio {//namespace zinx_asio

TCPConnection::TCPConnection(ConnectionServer<TCPConnection>* s,
                             boost::asio::io_context& ioc,
                             uint32_t id, size_t time, uint32_t maxPackageSize,
                             std::shared_ptr<ConnManager> cm,
                             std::shared_ptr<MessageManager> mm)
    : belongServer_(s), socket_(ioc), connID_(id),
      maxConnIdleTime_(time), isClosed_(false), connMgr_wptr(cm),
      routers_ptr(mm), strand_(ioc), timer_(ioc) {
    dataPack_.setMaxPackageSize(maxPackageSize);
}

TCPConnection::~TCPConnection () {}

//更新timer截止时间
void TCPConnection::setTimerCallback() {
    auto self(shared_from_this());
    //设置timer超时时间
    timer_.async_wait(
    [this, self](const boost::system::error_code & ec) {
        if (ec) {
            //如果有错误，应该是timer被cancel
            std::cout << "TCPConnection " << connID_ << " Timer Error: " << ec.message() << '\n';
        }
        //当前已经超时
        if(timer_.expiry() <= boost::asio::steady_timer::clock_type::now()) {
            timeOut_.store(true, std::memory_order_relaxed);
            std::cout << "TCPConnection " << connID_ << " Timeout" << '\n';
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
void TCPConnection::startRead(boost::asio::yield_context yield) {
    printf("[TCPConnection %d ReadHandler Start]\n", connID_);

    //创建拆解包对象
    //读取客户端户message head的数据流的前八个字节
    uint32_t len = dataPack_.getHeadLen();
    uint32_t id{0};
    char head[len];

    try {
        boost::asio::async_read(socket_, boost::asio::buffer(head, len),
                                boost::asio::transfer_exactly(len), yield);
        std::cout << "Server Read Data Head " << len << " Bytes" << std::endl;
    } catch(std::exception& ec) {
        std::cout << "[Read head exits error] " << ec.what() << std::endl;
        //读取错误或终止时
        stop();
        return;
    }

    //拆包:读取messageID和Len放进headData
    //解包错误,直接关闭客户端
    try {
        dataPack_.unpack(len, id, head);
    } catch(const std::exception& e) {
        std::cout << "[Data Unpack Error] " << e.what() << '\n';
        //读取错误或终止时
        stop();
    }

    auto msg = std::make_shared<TCPMessage>(id, len);
    try {
        boost::asio::async_read(socket_, msg->bufferRef().prepare(len),
                                boost::asio::transfer_exactly(len), yield);
        msg->bufferRef().commit(len);
        std::cout << "Server Read Data Body " << len << " Bytes" << std::endl;
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
            routers_ptr->doMsgHandler(self, msg);
            printf("[TCPConnection %d ReadHandler Stop]\n", connID_);
        });
    } else {
        //或直接执行
        routers_ptr->doMsgHandler(self, msg);
        printf("[TCPConnection %d ReadHandler Stop]\n", connID_);
    }

    if (maxConnIdleTime_ > 0) {
        timer_.expires_after(std::chrono::seconds(maxConnIdleTime_));
    }

    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
        startRead(yield);
    });
}

//start 启动链接
void TCPConnection::start() {
    printf("==================No.%d TCPConnection Start==================\n", connID_);
    auto self(shared_from_this());
    //调用OnConnStart
    belongServer_->callOnConnStart(self);

    if (maxConnIdleTime_ > 0) {
        //开始时设置timer存活时间
        timer_.expires_after(std::chrono::seconds(maxConnIdleTime_));
        setTimerCallback();
    }

    boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield) {
        startRead(yield);
    });
}

//stop 停止链接
void TCPConnection::stop() {
    if(isClosed_.load(std::memory_order_relaxed)) {
        return;
    }
    printf("==================No.%d TCPConnection Stop==================\n", connID_);
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
        std::cout << "No." << connID_ << " TCPConnection connMgr_wptr is nullptr" << std::endl;
    }
}

//------------------------------------------------------------------------------
//SendMsg 发送数据
void TCPConnection::sendMsg(const char* data, size_t size) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data, size), boost::asio::transfer_all(),
    [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

//SendMsg 发送数据
void TCPConnection::sendMsg(const std::vector<char>& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(const std::vector<char>* data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data->data(), data->size()),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(std::vector<char>&& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data.data(), data.size()),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(const std::shared_ptr<std::vector<char>> data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data->data(), data->size()),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

//SendMsg 发送数据
void TCPConnection::sendMsg(std::string&& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data.data(), data.size()),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(const std::string& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(const std::string* data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data->data(), data->size()),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(const std::shared_ptr<std::string> data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data->data(), data->size()),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

//SendMsg 发送数据
void TCPConnection::sendMsg(boost::asio::streambuf&& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data,
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(boost::asio::streambuf& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data,
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(boost::asio::streambuf* data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, *data,
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(std::shared_ptr<boost::asio::streambuf> data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << '\n';
            //读取错误或终止时
            self->stop();
            return;
        }
        data->consume(length);
    });
}

//SendMsg 发送数据
void TCPConnection::sendMsg(IMessage& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data.data(),
    boost::asio::transfer_all(), [self, &data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << '\n';
            //读取错误或终止时
            self->stop();
            return;
        }
        //减去头长
        data.consume(length - 8);
    });
}

void TCPConnection::sendMsg(IMessage&& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data.data(),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(IMessage* data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        //减去头长
        data->consume(length - 8);
    });
}

void TCPConnection::sendMsg(std::shared_ptr<IMessage> data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        //减去头长
        data->consume(length - 8);
    });
}

//SendMsg 发送数据
void TCPConnection::sendMsg(ByteBuffer<std::allocator<char>>&& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data.data(),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(ByteBuffer<std::allocator<char>>& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data.data(),
    boost::asio::transfer_all(), [self, &data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        data.consume(length);
    });
}

void TCPConnection::sendMsg(ByteBuffer<std::allocator<char>>* data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        data->consume(length);
    });
}

void TCPConnection::sendMsg(std::shared_ptr<ByteBuffer<std::allocator<char>>> data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        data->consume(length);
    });
}

//SendMsg 发送数据
void TCPConnection::sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>&& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data.data(),
    boost::asio::transfer_all(), [self](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
    });
}

void TCPConnection::sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>& data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data.data(),
    boost::asio::transfer_all(), [self, &data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        data.consume(length);
    });
}

void TCPConnection::sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>* data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        data->consume(length);
    });
}

void TCPConnection::sendMsg(std::shared_ptr<ByteBuffer<__gnu_cxx::__pool_alloc<char>>> data) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, data->data(),
    boost::asio::transfer_all(), [self, data](const boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "[sendMsg exits error] " << ec.message() << std::endl;
            //读取错误或终止时
            self->stop();
            return;
        }
        data->consume(length);
    });
}

//getSocket 获取当前连接绑定的socket
boost::asio::ip::tcp::socket& TCPConnection::getSocket() {
    return socket_;
}

//getConnID 获取当前连接的ID
uint32_t TCPConnection::getConnID() const {
    return connID_;
}

//getRemoteEndpoint 获取客户端的TCP状态IP和Port
boost::asio::ip::tcp::endpoint TCPConnection::getRemoteEndpoint() {
    return socket_.remote_endpoint();
}

//getLocalEndpoint 获取本地的TCP状态IP和Port
boost::asio::ip::tcp::endpoint TCPConnection::getLocalEndpoint() {
    return socket_.local_endpoint();
}

}//namespace zinx_asio
