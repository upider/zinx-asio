#include <iostream>
#include <thread>

#include <boost/bind.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/defer.hpp>
#include <boost/asio/dispatch.hpp>

#include "io_context_pool.hpp"

io_context_pool::io_context_pool(std::size_t pool_size, std::size_t ioc_size)
    : threadNum_(pool_size), iocNum_(ioc_size), threads_(), next_io_context_(0) {
    if (pool_size == 0)
        throw std::runtime_error("io_context_pool size is 0");

    // Give all the io_contexts work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    for (std::size_t i = 0; i < ioc_size; ++i) {
        boost::asio::io_context* io_context = new boost::asio::io_context(2);
        io_contexts_.push_back(io_context);
        work_.push_back(boost::asio::make_work_guard(*io_context));
    }
}

io_context_pool::~io_context_pool() {
    for (size_t i = 0; i < io_contexts_.size(); ++i) {
        io_contexts_[i]->stop();
        delete io_contexts_[i];
    }
}

void io_context_pool::run() {
    // Create a pool of threads to run all of the io_contexts.
    if (iocNum_ == threadNum_) {
        for (std::size_t i = 0; i < iocNum_; ++i) {
            threads_.create_thread(boost::bind(&boost::asio::io_context::run, io_contexts_[i]));
        }
    } else if (iocNum_ < threadNum_) {
        size_t i = 0;//io_contexts
        for (size_t j = 0; j < threadNum_; j++) {
            threads_.create_thread(boost::bind(&boost::asio::io_context::run, io_contexts_[i++]));
            if (i == iocNum_) {
                i = 0;
            }
        }
    } else {
        std::cout << "io_context num can't more than thread num" << std::endl;
    }
}

///get io_context num
size_t io_context_pool::iocNum()const {
    return iocNum_;
}

///get thread num
//TODO:解决boost::thread_group的bug:size()方法无效
size_t io_context_pool::threadNum()const {
    return threadNum_;
}

///join all threads
void io_context_pool::joinAll() {
    threads_.join_all();
}

void io_context_pool::stop() {
    // Explicitly stop all io_contexts.
    for (std::size_t i = 0; i < io_contexts_.size(); ++i)
        io_contexts_[i]->stop();
}

boost::asio::io_context& io_context_pool::getCtx() {
    // Use a round-robin scheme to choose the next io_context to use.
    boost::asio::io_context &io_context = *io_contexts_[next_io_context_];
    ++next_io_context_;
    if (next_io_context_ == io_contexts_.size())
        next_io_context_ = 0;
    return io_context;
}
