#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <memory>
#include <functional>
#include "class_factory.hpp"

namespace zinx_asio {//namespace zinx_asio

class Request;

class Router {
    public:
        Router();
        virtual ~Router ();
        virtual void preHandle(Request&) = 0;
        virtual void handle(Request&) = 0;
        virtual void postHandle(Request&) = 0;
};

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
