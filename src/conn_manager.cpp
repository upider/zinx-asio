#include <iostream>

#include "iconnection.hpp"
#include "conn_manager.hpp"

namespace zinx_asio {//namespace zinx_asio

ConnManager::ConnManager() {}

ConnManager::~ConnManager() {}

//add 添加连接
void ConnManager::addConn(Conn_ptr conn) {
    connsLock.lock();
    conns_[conn->getConnID()] = conn;
    connsLock.unlock();
    printf("==============%d Connection added to ConnManager, conn num = %lu==============\n",
           conn->getConnID(), conns_.size());
}

//del 删除链接
void ConnManager::delConn(Conn_ptr conn) {
    connsLock.lock();
    conns_.erase(conn->getConnID());
    connsLock.unlock();
    printf("============%d Connection removed from ConnManager, conn num = %lu==============\n",
           conn->getConnID(), conns_.size());
}

//get 根据ID获取连接
ConnManager::Conn_ptr ConnManager::getConn(uint32_t id) {
    connsLock.lock_shared();
    auto it = conns_.find(id);
    connsLock.unlock_shared();
    if (it != conns_.end()) {
        return it->second;
    }
    return nullptr;
}

//size 获取当前连接总数
uint32_t ConnManager::size() {
    connsLock.lock_shared();
    size_t s = conns_.size();
    connsLock.unlock_shared();
    return s;
}

//clear 清除所有连接
void ConnManager::clear() {
    connsLock.lock();
    for (auto it = conns_.begin(); it != conns_.end(); ++it) {
        it->second->stop();
    }
    conns_.clear();
    connsLock.unlock();
}

void ConnManager::stop() {
    stopped_.store(true, std::memory_order_relaxed);
}

bool ConnManager::isStopped() {
    return stopped_.load(std::memory_order_relaxed);
}
}//namespace zinx_asio
