#include <fstream>
#include <nlohmann/json.hpp>
#include "utils.hpp"

// for convenience
using json = nlohmann::json;

GlobalObject::GlobalObject() {
    //read a JSON file
    std::ifstream is("conf/zinx.json");
    json obj;
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
