#include <iostream>

#include "conn_manager.hpp"
#include "connection.hpp"

namespace zinx_asio {//namespace zinx_asio
ConnManager::ConnManager() {}
ConnManager::~ConnManager() {}

//add 添加连接
void ConnManager::addConn(Conn_ptr conn) {
    connsLock.lock();
    conns[conn->getConnID()] = conn;
    connsLock.unlock();
    printf("==============%d Connection added to ConnManager, conn num = %lu==============\n", conn->getConnID(), conns.size());
}

//del 删除链接
void ConnManager::delConn(Conn_ptr conn) {
    connsLock.lock();
    conns.erase(conn->getConnID());
    connsLock.unlock();
    printf("============%d Connection removed from ConnManager, conn num = %lu==============\n", conn->getConnID(), conns.size());
}

//get 根据ID获取连接
Conn_ptr ConnManager::getConn(uint32_t id) {
    connsLock.lock_shared();
    auto it = conns.find(id);
    connsLock.unlock_shared();
    if (it != conns.end()) {
        return it->second;
    }
    return nullptr;
}

//size 获取当前连接总数
uint32_t ConnManager::size() {
    connsLock.lock_shared();
    size_t s = conns.size();
    connsLock.unlock_shared();
    return s;
}

//clear 清除所有连接
void ConnManager::clear() {
    connsLock.lock();
    for (auto it = conns.begin(); it != conns.end(); ++it) {
        it->second->stop();
    }
    conns.clear();
    connsLock.unlock();
}

}//namespace zinx_asio
