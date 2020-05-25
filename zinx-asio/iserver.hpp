#ifndef ISERVER_HPP
#define ISERVER_HPP

#include <map>

class IServer {
    public:
        IServer() {}
        virtual ~IServer() {}
    public:
        //accept
        virtual void doAccept(size_t acceptorIndex) = 0;
        //start 开始
        virtual void start() = 0;
        //listen
        virtual void listen() = 0;
        //listen
        virtual void listen(std::map<std::string, int> endpoints) = 0;
        //stop 停止
        virtual void stop() = 0;
        //server 运行
        virtual void serve() = 0;
        //设置空闲连接时间
        virtual void setMaxConnIdleTime(std::size_t) = 0;
        //addRouter 添加路由
        virtual void addRouter(uint32_t, const std::string&) = 0;
};

#endif /* ISERVER_HPP */
