#ifndef IMESSAGE_HPP
#define IMESSAGE_HPP

#include <inttypes.h>
#include <boost/asio/buffer.hpp>
#include <list>

#include "byte_buffer.hpp"

namespace zinx_asio {//namespace zinx_asio

class IMessage {
    public:
        IMessage() {}
        virtual ~IMessage () {}

        virtual uint32_t getMsgID()const = 0;
        virtual uint32_t getMsgLen()const = 0;
        virtual void setMsgID(uint32_t) = 0;
        virtual void setMsgLen(uint32_t) = 0;
        //设置消  息数组
        virtual void setContent(const std::vector<char>&) = 0;
        virtual void setContent(const std::string&) = 0;
        virtual void setContent(const char*, size_t) = 0;

        virtual ByteBuffer<>& bufferRef() = 0;
        virtual std::string contentToString() = 0;

        virtual std::list<boost::asio::const_buffer> data() = 0;
        virtual void commit(std::size_t) = 0;
        virtual void consume(std::size_t) = 0;
};

}//namespace zinx_asio
#endif /* IMESSAGE_HPP */
