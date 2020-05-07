#ifndef IO_CONTEXT_POOL_HPP
#define IO_CONTEXT_POOL_HPP

#include <list>
#include <stdexcept>
#include <memory>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace zinx_asio {//namespace zinx_asio
/// A pool of io_context objects.
class io_context_pool : private boost::noncopyable {
    public:
        /// Construct the io_context pool.
        explicit io_context_pool(std::size_t pool_size, std::size_t ioc_size);
        /// Deconstruct
        virtual ~io_context_pool();
        /// Run all io_context objects in the pool.
        void run();
        /// Stop all io_context objects in the pool.
        void stop();
        /// Get an io_context to use.
        boost::asio::io_context &getCtx();
        ///join all threads
        void joinAll();
        ///get io_context num
        size_t iocNum()const;
        ///get thread num
        size_t threadNum()const;
        //TODO:整合post dispatch defer
        ///post task
        //void post();
        ///dispatch task
        //void dispatch ();
        ///defer task
        //void defer();

    private:
        typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_work;

        size_t threadNum_{0};
        size_t iocNum_{0};
        ///threads use to run io_context
        boost::thread_group threads_;
        /// The pool of io_contexts.
        std::vector<boost::asio::io_context*> io_contexts_;

        /// The work that keeps the io_contexts running.
        std::list<io_context_work> work_;

        /// The next io_context to use for a connection.
        std::size_t next_io_context_;
};

}//namespace zinx_asio
#endif /*IO_CONTEXT_POOL_HPP*/
