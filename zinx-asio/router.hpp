#ifndef ROUTER_HPP
#define ROUTER_HPP

class Request;

class Router {
    public:
        Router();
        virtual ~Router ();
        virtual void preHandle(Request&) = 0;
        virtual void handle(Request&) = 0;
        virtual void postHandle(Request&) = 0;
};

#endif /* ROUTER_HPP */
