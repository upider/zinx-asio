#ifndef MESSAGE_MANAGER_HPP
#define MESSAGE_MANAGER_HPP

#include <map>
#include <memory>

#include "imessage.hpp"
#include "router.hpp"

namespace zinx_asio {//namespace zinx_asio

class Datagram;
class IMessage;
class IConnection;

class MessageManager {
    public:
        ///默认构造
        MessageManager();
        virtual ~MessageManager();
        //doMsgHandler 调度或执行对应的TCP Router
        void doMsgHandler(std::shared_ptr<IConnection> conn,
                          std::shared_ptr<IMessage> msg) const;
        //doMsgHandler 调度或执行对应的UDP Router
        void doMsgHandler(std::shared_ptr<Datagram> datagram,
                          std::shared_ptr<IMessage> msg) const;
        //addRouter 添加消息执行对应的Router
        void addRouter(uint32_t id, std::shared_ptr<IRouter> router);
    private:
        //存放每个MsgID对应的处理方法
        std::map<uint32_t, std::shared_ptr<IRouter>> routerMap_;
};

}//namespace zinx_asio
#endif /* MESSAGE_MANAGER_HPP */
