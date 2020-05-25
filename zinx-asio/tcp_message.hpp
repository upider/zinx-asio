#ifndef TCPMESSAGE_HPP
#define TCPMESSAGE_HPP

#include "imessage.hpp"

namespace zinx_asio {//namespace zinx_asio

class TCPMessage: public IMessage {
    public:
        TCPMessage();
        TCPMessage(TCPMessage&&);
        TCPMessage(const TCPMessage&);
        TCPMessage(uint32_t, const char*, std::size_t);
        TCPMessage(uint32_t, std::size_t);
        TCPMessage& operator=(TCPMessage&&);
        TCPMessage& operator=(const TCPMessage&);
        virtual ~TCPMessage ();

        virtual uint32_t getMsgID()const;
        virtual uint32_t getMsgLen()const;
        virtual void setMsgID(uint32_t);
        virtual void setMsgLen(uint32_t);
        //设置消  息数组
        virtual void setContent(const std::vector<char>&);
        virtual void setContent(const std::string&);
        virtual void setContent(const char*, size_t);

        ByteBuffer<>& bufferRef();
        virtual std::string contentToString();
        //适配asio buffer
        virtual std::list<boost::asio::const_buffer> data();
        virtual void commit(std::size_t);
        virtual void consume(std::size_t);

    private:
        //len和ID
        char head_[9];
        //消息内容 数据
        ByteBuffer<> data_;
};

}//namespace zinx_asio
#endif /* TCPMESSAGE_HPP */
