#ifndef ISERVER_HPP
#define ISERVER_HPP

#include <map>

namespace zinx_asio {//namespace zinx_asio

class IServer {
    public:
        IServer() {}
        virtual ~IServer() {}
    public:
        //accept
        virtual void doAccept(size_t acceptorIndex) = 0;
        //start 开始
        virtual void start() = 0;
        //stop 停止
        virtual void stop() = 0;
        //server 运行
        virtual void serve() = 0;
        //addRouter 添加路由
        virtual void addRouter(uint32_t, const std::string&) = 0;
        //RouterMap 返回连接管理模块
        virtual std::map<uint32_t, std::string> getRouters() = 0;
};

}//namespace zinx_asio
#endif /* ISERVER_HPP */
