#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "utils.hpp"

namespace zinx_asio {//namespace zinx_asio

//Server配置
std::string GlobalObject::ServerName_{""}; //Server name
//监听的所有地址
std::multimap<std::string, int> GlobalObject::EndPoints_{};
//Connection最空闲大连接时间-秒,0表示不设置连接时间
uint32_t GlobalObject::MaxConnIdleTime_{0};

//zinx配置
std::string GlobalObject::ZinxVersion_{""}; //zinx版本号
uint32_t GlobalObject::MaxConnNum_{1024};           //服务器主机最大连接数
uint32_t GlobalObject::MaxPackageSize_{512};    //zinx数据包最大值
uint32_t GlobalObject::IOWorkerPoolSize_{1};    //IOWorker池的worker数量
uint32_t GlobalObject::TaskWorkerPoolSize_{0};    //任务worker池的worker数量
uint32_t GlobalObject::TaskWorkerQueueNum_{0}; //任务worker池的io_context数量

GlobalObject::GlobalObject() {}
GlobalObject::~GlobalObject() {}

//解析配置文件
void GlobalObject::parseConf(const std::string& conf) {
    //read a JSON file
    std::ifstream is(conf);
    nlohmann::json obj;
    is >> obj;

    ServerName_ = obj["ServerName"];

    ZinxVersion_ = obj["ZinxVersion"];
    MaxConnNum_ = obj["MaxConnNum"];
    MaxPackageSize_ = obj["MaxPackageSize"];
    IOWorkerPoolSize_ = obj["IOWorkerPoolSize"];
    TaskWorkerPoolSize_ = obj["TaskWorkerPoolSize"];
    TaskWorkerQueueNum_ = obj["TaskWorkerQueueNum"];
    MaxConnIdleTime_ = obj["MaxConnIdleTime"];
    for (size_t i = 0; i < obj["EndPoints"].size(); ++i) {
        std::string host = obj["EndPoints"][i]["Host"];
        int port = obj["EndPoints"][i]["Port"];
        EndPoints_.emplace(host, port);
    }
}

std::string GlobalObject::serverName() {
    return ServerName_;
}

//监听的所有地址
std::multimap<std::string, int> GlobalObject::endPoints() {
    return EndPoints_;
}

uint32_t GlobalObject::maxConnIdleTime() {
    return MaxConnIdleTime_;
}

uint32_t GlobalObject::maxConnNum() {
    return MaxConnNum_;
}

uint32_t GlobalObject::maxPackageSize() {
    return MaxPackageSize_;
}

//zinx配置
std::string GlobalObject::zinxVersion() {
    return ZinxVersion_;
}

//IOWorker池的worker数量
uint32_t GlobalObject::ioWorkerPoolSize() {
    return IOWorkerPoolSize_;
}

//任务worker池的worker数量
uint32_t GlobalObject::taskWorkerPoolSize() {
    return TaskWorkerPoolSize_;
}

//任务worker池的io_context数量
uint32_t GlobalObject::taskWorkerQueueNum() {
    return TaskWorkerQueueNum_;
}

//设置配置
//服务器名称
void GlobalObject::serverName(const std::string& name) {
    ServerName_ = name;
}

//监听的所有地址
void GlobalObject::addEndPoint(const std::string& host, int port) {
    EndPoints_.emplace(host, port);
}

//返回最大空闲连接时间
void GlobalObject::maxConnIdleTime(uint32_t time) {
    MaxConnIdleTime_ = time;
}

//返回最大连接数
void GlobalObject::maxConnNum(uint32_t num) {
    MaxConnNum_ = num;
}

//返回最大包大小
void GlobalObject::maxPackageSize(uint32_t size) {
    MaxPackageSize_ = size;
}

//zinx配置
void GlobalObject::zinxVersion(const std::string& version) {
    ZinxVersion_ = version;
}

//IOWorker池的worker数量
void GlobalObject::ioWorkerPoolSize(uint32_t size) {
    IOWorkerPoolSize_ = size;
}

//任务worker池的worker数量
void GlobalObject::taskWorkerPoolSize(uint32_t size) {
    TaskWorkerPoolSize_ = size;
}

//任务worker池的io_context数量
void GlobalObject::taskWorkerQueueNum(uint32_t num) {
    TaskWorkerQueueNum_ = num;
}

}//namespace zinx_asio
