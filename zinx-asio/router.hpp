#ifndef ROUTER_HPP
#define ROUTER_HPP

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

}//namespace zinx_asio
#endif /* ROUTER_HPP */
