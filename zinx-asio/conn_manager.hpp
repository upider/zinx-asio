#ifndef CONN_MANAGER_HPP
#define CONN_MANAGER_HPP

#include <atomic>
#include <map>
#include <memory>
#include <boost/thread/shared_mutex.hpp>

namespace zinx_asio {//namespace zinx_asio
class Connection;
typedef std::shared_ptr<Connection> Conn_ptr;

class ConnManager {
    public:
        ConnManager();
        virtual ~ConnManager ();
        //add 添加连接
        void addConn(Conn_ptr);
        //del 删除链接
        void delConn(Conn_ptr);
        //get 根据ID获取连接
        Conn_ptr getConn(uint32_t);
        //size 获取当前连接总数
        uint32_t size();
        //clear 清除所有连接
        void clear();
    private:
        std::map<uint32_t, Conn_ptr> conns;
        boost::shared_mutex connsLock;
};

}//namespace zinx_asio
#endif /* CONN_MANAGER_HPP */
