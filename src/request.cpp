#include "request.hpp"

namespace zinx_asio {//namespace zinx_asio
Request::Request(Message msg, std::shared_ptr<Connection> conn)
    : msg_(msg), conn_ptr(conn) {}

Request::Request() {}

Request::~Request() {}

uint32_t Request::getMsgID() const {
    return msg_.getMsgID();
}

const std::vector<char>& Request::getData() const {
    return msg_.getData();
}

std::shared_ptr<Connection> Request::getConnection() {
    return conn_ptr;
}

}//namespace zinx_asio
