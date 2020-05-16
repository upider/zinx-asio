#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <list>
#include <boost/asio/ip/tcp.hpp>

#include "io_context_pool.hpp"

namespace zinx_asio {//namespace zinx_asio

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
        //监听的所有地址
        std::list<boost::asio::ip::tcp::endpoint> EndPoints;
        //Connection最大连接时间-秒,0表示不设置连接时间
        size_t MaxConnIdleTime;

        //zinx配置
        std::string ZinxVersion; //zinx版本号
        uint32_t MaxConnNum;           //服务器主机最大连接数
        uint32_t MaxPackageSize;    //zinx数据包最大值
        uint32_t IOWorkerPoolSize;    //IOWorker池的worker数量
        uint32_t TaskWorkerPoolSize;    //任务worker池的worker数量
        uint32_t TaskWorkerQueueNum; //任务worker池的io_context数量
};

}//namespace zinx_asio
#endif /* UTILS_HPP */
