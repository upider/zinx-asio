#include "router.hpp"

namespace zinx_asio {//namespace zinx_asio

//----------------------------------------------------------------
IRouter::IRouter() {}
IRouter::~IRouter() {}

//----------------------------------------------------------------
ConnectionRouter::ConnectionRouter() {}
ConnectionRouter::~ConnectionRouter() {}

void ConnectionRouter::preHandle(std::shared_ptr<Datagram>,
                                 std::shared_ptr<IMessage> msg) {}
void ConnectionRouter::handle(std::shared_ptr<Datagram>,
                              std::shared_ptr<IMessage> msg) {}
void ConnectionRouter::postHandle(std::shared_ptr<Datagram>,
                                  std::shared_ptr<IMessage> msg) {}

//----------------------------------------------------------------
DatagramRouter::DatagramRouter() {}
DatagramRouter::~DatagramRouter() {}

void DatagramRouter::preHandle(std::shared_ptr<IConnection> conn,
                               std::shared_ptr<IMessage> msg) {}
void DatagramRouter::handle(std::shared_ptr<IConnection> conn,
                            std::shared_ptr<IMessage> msg) {}
void DatagramRouter::postHandle(std::shared_ptr<IConnection> conn,
                                std::shared_ptr<IMessage> msg) {}

}//namespace zinx_asio
