#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "message.hpp"
#include "connection.hpp"

class Request {
    public:
        Request(Message msg, std::shared_ptr<Connection> conn);
        Request();
        ~Request();
        uint32_t getMsgID() const;
        std::vector<char>& getData();
        std::shared_ptr<Connection> getConnection();
    private:
        Message msg_;
        std::shared_ptr<Connection> conn_ptr;
};

#endif /* REQUEST_HPP */
