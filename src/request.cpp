#include "request.hpp"

namespace zinx_asio {//namespace zinx_asio
Request::Request(std::shared_ptr<Message> msg, std::shared_ptr<Connection> conn)
    : msg_(msg), conn_ptr(conn) {}

Request::Request() {}

Request::~Request() {}

std::shared_ptr<Message> Request::getMsg() {
    return msg_;
}

uint32_t Request::getMsgID() {
    return msg_->getMsgID();
}

uint32_t Request::getMsgLen() {
    return msg_->getMsgLen();
}

std::shared_ptr<Connection> Request::getConnection() {
    return conn_ptr;
}

}//namespace zinx_asio
