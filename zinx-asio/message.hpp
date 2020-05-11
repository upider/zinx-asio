#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <inttypes.h>

#include "byte_buffer.hpp"

namespace zinx_asio {//namespace zinx_asio

class Message {
    public:
        Message(uint32_t, const char*, size_t);
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
        //得到ByteBuffer
        ByteBuffer<>& getData();

    private:
        //数据
        ByteBuffer<> data_;
        //消息ID
        uint32_t id_{0};
        //消息长度
        uint32_t len_{0};
        //消息内容
};

}//namespace zinx_asio
#endif /* MESSAGE_HPP */
