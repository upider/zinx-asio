#include <iostream>

#include "datagram.hpp"

namespace zinx_asio {//namespace zinx_asio

Datagram::Datagram(std::shared_ptr<boost::asio::ip::udp::socket> sock)
    : socket_(sock) {}

Datagram::Datagram(std::shared_ptr<boost::asio::ip::udp::socket> sock,
                   boost::asio::ip::udp::endpoint point)
    : socket_(sock), sendPoint_(point) {}

Datagram::~Datagram() {}

void Datagram::setEndPoint(boost::asio::ip::udp::endpoint point) {
    sendPoint_ = point;
}

boost::asio::ip::udp::endpoint& Datagram::getEndPoint() {
    return sendPoint_;
}

//getSocket 获取当前连接绑定的socket
std::shared_ptr<boost::asio::ip::udp::socket> Datagram::getSocket() {
    return socket_;
}

//SendMsg 发送数据
void Datagram::sendMsg(const char* data, std::size_t size) {
    socket_->async_send_to(boost::asio::buffer(data, size), sendPoint_,
    [](const boost::system::error_code & ec, std::size_t length) {
        if (ec) {
            std::cout << "SendMsg Error: " << ec.message() << '\n';
        }
    });
}

//SendMsg 发送数据
void Datagram::sendMsg(const std::vector<char>&) {}
void Datagram::sendMsg(std::vector<char>&&) {}
void Datagram::sendMsg(const std::vector<char>*) {}
void Datagram::sendMsg(const std::shared_ptr<std::vector<char>>) {}

//SendMsg 发送数据
void Datagram::sendMsg(const std::string& str) {
    socket_->async_send_to(boost::asio::buffer(str), sendPoint_,
    [](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "SendMsg Error: " << ec.message() << '\n';
        }
    });
}

void Datagram::sendMsg(std::string&& str) {
    socket_->async_send_to(boost::asio::buffer(str), sendPoint_,
    [](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "SendMsg Error: " << ec.message() << '\n';
        }
    });
}

void Datagram::sendMsg(const std::string* str) {
    socket_->async_send_to(boost::asio::buffer(str->data(), str->size()), sendPoint_,
    [](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "SendMsg Error: " << ec.message() << '\n';
        }
    });
}

void Datagram::sendMsg(const std::shared_ptr<std::string> str) {
    socket_->async_send_to(boost::asio::buffer(str->data(), str->size()), sendPoint_,
    [](boost::system::error_code ec, std::size_t length) {
        if (ec) {
            std::cout << "SendMsg Error: " << ec.message() << '\n';
        }
    });
}

//SendMsg 发送数据
void Datagram::sendMsg(boost::asio::streambuf&) {}
void Datagram::sendMsg(boost::asio::streambuf&&) {}
void Datagram::sendMsg(boost::asio::streambuf*) {}
void Datagram::sendMsg(std::shared_ptr<boost::asio::streambuf>) {}
//SendMsg 发送数据
void Datagram::sendMsg(IMessage&) {}
void Datagram::sendMsg(IMessage&&) {}
void Datagram::sendMsg(IMessage*) {}
void Datagram::sendMsg(std::shared_ptr<IMessage>) {}
//SendMsg 发送数据
void Datagram::sendMsg(ByteBuffer<std::allocator<char>>& buffer) {}
void Datagram::sendMsg(ByteBuffer<std::allocator<char>>* buffer) {}
void Datagram::sendMsg(ByteBuffer<std::allocator<char>>&& buffer) {}
void Datagram::sendMsg(std::shared_ptr<ByteBuffer<std::allocator<char>>> buffer) {}
void Datagram::sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>& buffer) {}
void Datagram::sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>* buffer) {}
void Datagram::sendMsg(ByteBuffer<__gnu_cxx::__pool_alloc<char>>&& buffer) {}
void Datagram::sendMsg(std::shared_ptr<ByteBuffer<__gnu_cxx::__pool_alloc<char>>> buffer) {}

}//namespace zinx_asio
