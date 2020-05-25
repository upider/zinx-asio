#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <memory>
#include <functional>

#include "class_factory.hpp"
#include "iconnection.hpp"

namespace zinx_asio {//namespace zinx_asio

class IMessage;

class Router {
    public:
        Router();
        virtual ~Router ();
        virtual void preHandle(std::shared_ptr<IConnection> conn,
                               std::shared_ptr<IMessage> msg) = 0;
        virtual void handle(std::shared_ptr<IConnection> conn,
                            std::shared_ptr<IMessage> msg) = 0;
        virtual void postHandle(std::shared_ptr<IConnection> conn,
                                std::shared_ptr<IMessage> msg) = 0;
};

/*class Router {*/
//    public:
//        Router();
//        virtual ~Router ();
//        template<typename ConnectionType>
//        void preHandle(std::shared_ptr<ConnectionType> conn,
//                       std::shared_ptr<Message> msg) {}
//        template<typename ConnectionType>
//        void handle(std::shared_ptr<ConnectionType> conn,
//                    std::shared_ptr<Message> msg) {}
//        template<typename ConnectionType>
//        void postHandle(std::shared_ptr<ConnectionType> conn,
//                        std::shared_ptr<Message> msg) {}
//};

typedef ClassFactory<std::shared_ptr<Router>,
        uint32_t,
        std::function<std::shared_ptr<Router>(void)>>
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
