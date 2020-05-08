#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "utils.hpp"

namespace zinx_asio {//namespace zinx_asio

GlobalObject::GlobalObject() {
    //read a JSON file
    std::ifstream is("conf/zinx.json");
    nlohmann::json obj;
    is >> obj;

    Name = obj["Name"];

    ZinxVersion = obj["ZinxVersion"];
    MaxConn = obj["MaxConn"];
    MaxPackageSize = obj["MaxPackageSize"];
    IOWorkerPoolSize = obj["IOWorkerPoolSize"];
    TaskWorkerPoolSize = obj["TaskWorkerPoolSize"];
    TaskWorkerQueueNum = obj["TaskWorkerQueueNum"];
    MaxConnTime = obj["MaxConnTime"];
    for (size_t i = 0; i < obj["EndPoints"].size(); ++i) {
        std::string host = obj["EndPoints"][i]["Host"];
        int port = obj["EndPoints"][i]["TCPPort"];
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);
        EndPoints.push_back(endpoint);
    }
}

GlobalObject::~GlobalObject() {}

}//namespace zinx_asio
