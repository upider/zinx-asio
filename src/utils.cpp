#include <fstream>
#include <nlohmann/json.hpp>
#include "utils.hpp"

namespace zinx_asio {//namespace zinx_asio

GlobalObject::GlobalObject() {
    //read a JSON file
    std::ifstream is("conf/zinx.json");
    nlohmann::json obj;
    is >> obj;

    Name = obj["Name"];
    Host = obj["Host"];
    TCPPort = obj["TCPPort"];

    ZinxVersion = obj["ZinxVersion"];
    MaxConn = obj["MaxConn"];
    MaxPackageSize = obj["MaxPackageSize"];
    IOWorkerPoolSize = obj["IOWorkerPoolSize"];
    TaskWorkerPoolSize = obj["TaskWorkerPoolSize"];
    TaskWorkerQueueNum = obj["TaskWorkerQueueNum"];
}

GlobalObject::~GlobalObject() {}

}//namespace zinx_asio
