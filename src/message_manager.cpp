#include <thread>
#include <iostream>

#include <boost/bind.hpp>
#include "request.hpp"
#include "message_manager.hpp"
#include "utils.hpp"

MessageManager::MessageManager() {}

MessageManager::~MessageManager() {}

//doMsgHandler 调度或执行对应的Router
void MessageManager::doMsgHandler(Request& request) const {
    //找到router
    auto it = routerMap_.find(request.getMsgID());
    if (it == routerMap_.end()) {
        printf("msgID = %d dose not have a router\n", request.getMsgID());
        return;
    }
    it->second->preHandle(request);
    it->second->handle(request);
    it->second->postHandle(request);
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
