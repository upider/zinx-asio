#include <iostream>

#include <boost/bind.hpp>
#include "message_manager.hpp"
#include "utils.hpp"

namespace zinx_asio {//namespace zinx_asio

///默认构造
MessageManager::MessageManager() {}
MessageManager::~MessageManager() {}

//doMsgHandler 调度或执行对应的IRouter
void MessageManager::doMsgHandler(std::shared_ptr<IConnection> conn,
                                  std::shared_ptr<IMessage> msg) const {
    for (auto& r : routerMap_) {
        try {
            r.second->preHandle(conn, msg);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
        try {
            r.second->handle(conn, msg);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
        try {
            r.second->postHandle(conn, msg);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
    }
}

//doMsgHandler 调度或执行对应的UDP IRouter
void MessageManager::doMsgHandler(std::shared_ptr<Datagram> endpoint,
                                  std::shared_ptr<IMessage> msg) const {
    for (auto& r : routerMap_) {
        try {
            r.second->preHandle(endpoint, msg);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
        try {
            r.second->handle(endpoint, msg);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
        try {
            r.second->postHandle(endpoint, msg);
        } catch(std::exception& e) {
            std::cout << "DoMsgHandler Error: " << e.what() << std::endl;
        }
    }
}

//addRouter 添加消息执行对应的IRouter
void MessageManager::addRouter(uint32_t id, std::shared_ptr<IRouter> router) {
    auto it = routerMap_.find(id);
    if (it != routerMap_.end()) {
        printf("Router has been added\n");
        return;
    }
    routerMap_[id] = router;
    printf("Router has been added\n");
}

}//namespace zinx_asio
