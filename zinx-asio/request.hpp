#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "message.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio

class Request {
    public:
        Request(std::shared_ptr<Message> msg, std::shared_ptr<Connection> conn);
        Request();
        ~Request();
        std::shared_ptr<Connection> getConnection();
        uint32_t getMsgID();
        uint32_t getMsgLen();
        std::shared_ptr<Message> getMsg();
    private:
        std::shared_ptr<Message> msg_;
        std::shared_ptr<Connection> conn_ptr;
};

}//namespace zinx_asio
#endif /* REQUEST_HPP */
