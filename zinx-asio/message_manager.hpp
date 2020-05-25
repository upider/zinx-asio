#ifndef MESSAGE_MANAGER_HPP
#define MESSAGE_MANAGER_HPP

#include <map>
#include <thread>
#include <memory>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include "imessage.hpp"
#include "router.hpp"

namespace zinx_asio {//namespace zinx_asio

class IMessage;
class IConnection;

class MessageManager {
    public:
        ///默认构造
        MessageManager();
        virtual ~MessageManager();
        //doMsgHandler 调度或执行对应的Router
        void doMsgHandler(std::shared_ptr<IConnection> conn,
                          std::shared_ptr<IMessage> msg) const;
        //addRouter 添加消息执行对应的Router
        void addRouter(uint32_t msgID, std::shared_ptr<Router> router);
    private:
        //存放每个MsgID对应的处理方法
        std::map<uint32_t, std::shared_ptr<Router>> routerMap_;
};

}//namespace zinx_asio
#endif /* MESSAGE_MANAGER_HPP */
