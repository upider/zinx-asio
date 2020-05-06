#include "request.hpp"

Request::Request(Message msg, std::shared_ptr<Connection> conn)
    : msg_(msg), conn_ptr(conn) {}

Request::Request() {}

Request::~Request() {}

uint32_t Request::getMsgID() const {
    return msg_.getMsgID();
}

std::vector<char>& Request::getData() {
    return msg_.getData();
}

std::shared_ptr<Connection> Request::getConnection() {
    return conn_ptr;
}
