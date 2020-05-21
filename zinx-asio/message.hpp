#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <inttypes.h>

#include "byte_buffer_stream.hpp"

namespace zinx_asio {//namespace zinx_asio

class Message {
    public:
        Message(uint32_t, const char*, std::size_t);
        Message(uint32_t, std::size_t);
        Message& operator=(Message&&);
        Message();
        virtual ~Message ();
        uint32_t getMsgID()const;
        uint32_t getMsgLen()const;
        void setMsgID(uint32_t);
        void setMsgLen(uint32_t);
        //设置消息数组
        void setData(std::vector<char>&);
        //设置消息数组
        void setData(const char*, size_t);
        //得到ByteBufferStream
        ByteBufferStream<>& getData();

    private:
        //消息ID
        uint32_t id_{0};
        //消息长度
        uint32_t len_{0};
        //消息内容 数据
        ByteBufferStream<> data_;
};

}//namespace zinx_asio
#endif /* MESSAGE_HPP */
