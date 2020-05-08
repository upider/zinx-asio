#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "message.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio

class Request {
    public:
        Request(Message msg, std::shared_ptr<Connection> conn);
        Request();
        ~Request();
        uint32_t getMsgID() const;
        const std::vector<char>& getData() const;
        std::shared_ptr<Connection> getConnection();
    private:
        Message msg_;
        std::shared_ptr<Connection> conn_ptr;
};

}//namespace zinx_asio
#endif /* REQUEST_HPP */
