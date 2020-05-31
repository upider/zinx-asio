#ifndef CONN_MANAGER_HPP
#define CONN_MANAGER_HPP

#include <memory>
#include <boost/thread/shared_mutex.hpp>

namespace zinx_asio {//namespace zinx_asio

class IConnection;
class ConnManager {
    public:
        using Conn_ptr = typename std::shared_ptr<IConnection>;
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
        void stop();
        bool isStopped();

    private:
        //连接集合
        std::map<uint32_t, std::shared_ptr<IConnection>> conns_;
        //连接集合的保护锁
        boost::shared_mutex connsLock;
        std::atomic_bool stopped_{false};
};

}//namespace zinx_asio
#endif /* CONN_MANAGER_HPP */
