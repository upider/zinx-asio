#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <memory>
#include <functional>
#include <boost/asio/ip/udp.hpp>

#include "class_factory.hpp"

namespace zinx_asio {//namespace zinx_asio

class Datagram;
class IMessage;
class IConnection;

class IRouter {
    public:
        IRouter();
        virtual ~IRouter();
        //tcp message handler
        virtual void preHandle(std::shared_ptr<IConnection> conn,
                               std::shared_ptr<IMessage> msg) = 0;
        virtual void handle(std::shared_ptr<IConnection> conn,
                            std::shared_ptr<IMessage> msg) = 0;
        virtual void postHandle(std::shared_ptr<IConnection> conn,
                                std::shared_ptr<IMessage> msg) = 0;

        //udp message handler
        virtual void preHandle(std::shared_ptr<Datagram>,
                               std::shared_ptr<IMessage> msg) = 0;
        virtual void handle(std::shared_ptr<Datagram>,
                            std::shared_ptr<IMessage> msg) = 0;
        virtual void postHandle(std::shared_ptr<Datagram>,
                                std::shared_ptr<IMessage> msg) = 0;
};

class ConnectionRouter: public IRouter {
    public:
        ConnectionRouter();
        virtual ~ConnectionRouter();

        //tcp message handler
        virtual void preHandle(std::shared_ptr<IConnection> conn,
                               std::shared_ptr<IMessage> msg) = 0;
        virtual void handle(std::shared_ptr<IConnection> conn,
                            std::shared_ptr<IMessage> msg) = 0;
        virtual void postHandle(std::shared_ptr<IConnection> conn,
                                std::shared_ptr<IMessage> msg) = 0;

    private:
        //udp message handler
        virtual void preHandle(std::shared_ptr<Datagram>,
                               std::shared_ptr<IMessage> msg);
        virtual void handle(std::shared_ptr<Datagram>,
                            std::shared_ptr<IMessage> msg);
        virtual void postHandle(std::shared_ptr<Datagram>,
                                std::shared_ptr<IMessage> msg);
};

class DatagramRouter: public IRouter {
    public:
        DatagramRouter();
        virtual ~DatagramRouter();

	public:
		//udp message handler
        virtual void preHandle(std::shared_ptr<IConnection> conn,
                               std::shared_ptr<IMessage> msg);
        virtual void handle(std::shared_ptr<IConnection> conn,
                            std::shared_ptr<IMessage> msg);
        virtual void postHandle(std::shared_ptr<IConnection> conn,
                                std::shared_ptr<IMessage> msg);

	private:
		//tcp message handler
        virtual void preHandle(std::shared_ptr<Datagram>,
                               std::shared_ptr<IMessage> msg) = 0;
        virtual void handle(std::shared_ptr<Datagram>,
                            std::shared_ptr<IMessage> msg) = 0;
        virtual void postHandle(std::shared_ptr<Datagram>,
                                std::shared_ptr<IMessage> msg) = 0;
};

typedef ClassFactory<std::shared_ptr<IRouter>,
        uint32_t,
        std::function<std::shared_ptr<IRouter>(void)>>
        RouterClassFactory;

}//namespace zinx_asio

#define RegisterRouterClass(RouterNum,xxx){\
	zinx_asio::RouterClassFactory::Instance() \
    .Register(RouterNum,[](){return std::make_shared<xxx>();}); \
}

#define UnregisterRouterClass(RouterNum){\
	zinx_asio::RouterClassFactory::Instance() \
    .Unregister(RouterNum) \
}

#define CreateRouterObject(RouterNum){\
	zinx_asio::RouterClassFactory::Instance() \
    .CreateObject(RouterNum) \
}

#endif /* ROUTER_HPP */
