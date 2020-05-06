#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>    // std::cout, std::endl
#include <chrono>      // std::chrono::seconds
#include <functional>  // std::bind
#include <utility>     // std::forward
#include <boost/asio.hpp>
#include <boost/range.hpp>
#include <boost/asio/spawn.hpp>

#include "io_context_pool.hpp"

struct GlobalObject {
    private:
        GlobalObject();
        virtual ~GlobalObject();
    public:
        static GlobalObject& getInstance() {
            //返回local static的引用
            static GlobalObject instance;
            return instance;
        }
    public:
        GlobalObject(const GlobalObject&) = delete;
        GlobalObject& operator=(const GlobalObject&) = delete;

    public:
        //Server配置
        std::string Name; //Server name
        //TCPServer ziface.IServer //全局Server
        std::string Host; //服务器主机监听的IP
        int TCPPort; //主机监听端口号

        //zinx配置
        std::string ZinxVersion; //zinx版本号
        uint32_t MaxConn;           //服务器主机最大连接数
        uint32_t MaxPackageSize;    //zinx数据包最大值
        uint32_t IOWorkerPoolSize;    //IOWorker池的worker数量
        uint32_t TaskWorkerPoolSize;    //任务worker池的worker数量
        uint32_t TaskWorkerQueueNum; //任务worker池的io_context数量
};

#endif /* UTILS_HPP */
