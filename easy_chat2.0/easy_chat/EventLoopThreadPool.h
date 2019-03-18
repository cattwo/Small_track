//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_EVENTLOOPTHREADPOOL_H
#define EASY_CHAT_EVENTLOOPTHREADPOOL_H

#include "noncopyable.h"
#include <functional>
#include <memory>
#include <vector>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* baseloop, const std::string& name);

    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads;}

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return started_;}

    const std::string& name() const { return name_;}

private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;

};



#endif //EASY_CHAT_EVENTLOOPTHREADPOOL_H
