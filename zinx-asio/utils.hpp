#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <boost/asio/ip/tcp.hpp>

#include "io_context_pool.hpp"

namespace zinx_asio {//namespace zinx_asio

class GlobalObject {
    public:
        GlobalObject();
        ~GlobalObject();

        //返回配置
        //返回服务器名称
        static std::string serverName();
        //监听的所有地址
        static std::multimap<std::string, int> endPoints();
        //返回最大空闲连接时间
        static uint32_t maxConnIdleTime();
        //返回最大连接数
        static uint32_t maxConnNum();
        //返回最大包大小
        static uint32_t maxPackageSize();
        //zinx配置
        static std::string zinxVersion();
        //IOWorker池的worker数量
        static uint32_t ioWorkerPoolSize();
        //任务worker池的worker数量
        static uint32_t taskWorkerPoolSize();
        //任务worker池的io_context数量
        static uint32_t taskWorkerQueueNum();
        //设置配置
        //服务器名称
        static void serverName(const std::string&);
        //监听的所有地址
        static void addEndPoint(const std::string&, int);
        //返回最大空闲连接时间
        static void maxConnIdleTime(uint32_t);
        //返回最大连接数
        static void maxConnNum(uint32_t);
        //返回最大包大小
        static void maxPackageSize(uint32_t);
        //zinx配置
        static void  zinxVersion(const std::string&);
        //IOWorker池的worker数量
        static void ioWorkerPoolSize(uint32_t);
        //任务worker池的worker数量
        static void taskWorkerPoolSize(uint32_t);
        //任务worker池的io_context数量
        static void taskWorkerQueueNum(uint32_t);

    public:
        GlobalObject(const GlobalObject&) = delete;
        GlobalObject& operator=(const GlobalObject&) = delete;

    private:
        //Server配置
        static std::string ServerName_; //Server name
        static std::multimap<std::string, int> EndPoints_;
        //Connection最空闲大连接时间-秒,0表示不设置连接时间
        static uint32_t MaxConnIdleTime_;

        //zinx配置
        static std::string ZinxVersion_; //zinx版本号
        static uint32_t MaxConnNum_;           //服务器主机最大连接数
        static uint32_t MaxPackageSize_;    //zinx数据包最大值
        static uint32_t IOWorkerPoolSize_;    //IOWorker池的worker数量
        static uint32_t TaskWorkerPoolSize_;    //任务worker池的worker数量
        static uint32_t TaskWorkerQueueNum_; //任务worker池的io_context数量
};

}//namespace zinx_asio
#endif /* UTILS_HPP */
