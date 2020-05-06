#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <inttypes.h>

class Message {
    public:
        Message(uint32_t, const char*, size_t);
        Message();
        virtual ~Message ();
        //getMsgLen 获取消息长度
        uint32_t getMsgLen()const;
        //getMsgID 获取消息ID
        uint32_t getMsgID()const;
        //setMsgLen 设置消息长度
        void setMsgLen(uint32_t);
        //setMsgID 设置消息ID
        void setMsgID(uint32_t);
        //获取消息数组
        std::vector<char>& getData();
        //设置消息数组
        void setData(std::vector<char>&);
        //设置消息数组
        void setData(const char*, size_t);
		//=
		Message& operator=(const Message&);
    private:
        //消息ID
        uint32_t id_;
        //消息长度
        uint32_t msgLen_{0};
        //消息内容
        std::vector<char> data_;
};

#endif /* MESSAGE_HPP */
