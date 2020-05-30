#ifndef UDP_MESSAGE_HPP
#define UDP_MESSAGE_HPP

#include "imessage.hpp"
#include "byte_buffer.hpp"

namespace zinx_asio {//namespace zinx_asio

class UDPMessage: public IMessage {
    public:
        UDPMessage();
        UDPMessage(std::size_t);
        virtual ~UDPMessage();

    private:
        virtual uint32_t getMsgID()const;
        virtual void setMsgID(uint32_t);

    public:
        virtual uint32_t getMsgLen()const;
        virtual void setMsgLen(uint32_t);
        //设置消  息数组
        virtual void setContent(const std::vector<char>&);
        virtual void setContent(const std::string&);
        virtual void setContent(const char*, size_t);

        virtual ByteBuffer<>& bufferRef();
        virtual std::string contentToString();

        virtual std::list<boost::asio::const_buffer> data();
        virtual void commit(std::size_t);
        virtual void consume(std::size_t);
    private:
        uint32_t len_;
        ByteBuffer<> buffer_;
};

}//namespace zinx_asio
#endif /* UDP_MESSAGE_HPP */
