

#ifndef cppkit_actor_h
#define cppkit_actor_h

#include <mutex>
#include <condition_variable>
#include <future>
#include <list>
#include "cppkit/ck_logger.h"

namespace cppkit
{

/// An object that inherits from ck_actor is an entity with a thread that responds to commands
/// sent to it (in the order they were sent) producing some kind of result. ck_actor is a template
/// class that parameterizes both the command and result types. In addition, ck_actor returns a
/// std::future<> from post(), thus allowing clients the freedom to choose whether to block until
/// their command has a response or keep running until it does.

template<class CMD, class RESULT>
class ck_actor
{
 public:
    ck_actor() = default;
    ck_actor(const ck_actor&) = delete;
    ck_actor(ck_actor&&) noexcept = delete;

    virtual ~ck_actor() noexcept
    {
        if(started())
            stop();
    }

    ck_actor& operator=(const ck_actor&) = delete;
    ck_actor& operator=(ck_actor&&) noexcept = delete;

    void start()
    {
        std::unique_lock<std::mutex> g(_lock);
        _started = true;
        _thread = std::thread(&ck_actor<CMD,RESULT>::_main_loop, this);
    }

    inline bool started() const
    {
        return _started;
    }

    void stop()
    {
        if( started() )
        {
            {
                std::unique_lock<std::mutex> g(_lock);
                _started = false;
                _cond.notify_one();
                _queue.clear();
            }

            _thread.join();
        }
    }

    std::future<RESULT> post(const CMD& cmd)
    {
        std::unique_lock<std::mutex> g(_lock);

        std::promise<RESULT> p;
        auto waiter = p.get_future();

        _queue.push_front(std::pair<CMD,std::promise<RESULT>>(cmd, std::move(p)));

        _cond.notify_one();

        return waiter;
    }

    virtual RESULT process(const CMD& cmd) = 0;

protected:
    void _main_loop()
    {
        while(_started)
        {
            std::pair<CMD,std::promise<RESULT>> item;

            {
                std::unique_lock<std::mutex> g(_lock);

                _cond.wait(g, [this] () { return !this->_queue.empty() || !this->_started; });

                if(!_started)
                    continue;

                item = std::move(_queue.back());
                _queue.pop_back();
            }

            try
            {
                item.second.set_value(process(item.first));
            }
            catch(...)
            {
                try
                {
                    _queue.back().second.set_exception(std::current_exception());
                }
                catch(...)
                {
                    CK_LOG_NOTICE("Failed to attach unknown exception to std::promise<>");
                }
            }
        }
    }

    std::thread _thread;
    std::mutex _lock;
    std::condition_variable _cond;
    std::list<std::pair<CMD,std::promise<RESULT>>> _queue;
    bool _started = false;
};

}

#endif
