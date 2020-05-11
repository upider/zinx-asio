#include <thread>
#include <iostream>

#include <boost/bind.hpp>
#include "request.hpp"
#include "message_manager.hpp"
#include "utils.hpp"

namespace zinx_asio {//namespace zinx_asio

///默认构造
MessageManager::MessageManager() {}
MessageManager::~MessageManager() {}

//doMsgHandler 调度或执行对应的Router
void MessageManager::doMsgHandler(Request& request) const {
    for (auto& r : routerMap_) {
        try {
            r.second->preHandle(request);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
        try {
            r.second->handle(request);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
        try {
            r.second->postHandle(request);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
    }
}

//addRouter 添加消息执行对应的Router
void MessageManager::addRouter(uint32_t msgID, std::shared_ptr<Router> router) {
    auto it = routerMap_.find(msgID);
    if (it != routerMap_.end()) {
        printf("Router has been added\n");
        return;
    }
    routerMap_[msgID] = router;
    printf("Router has been added\n");
}

}//namespace zinx_asio
