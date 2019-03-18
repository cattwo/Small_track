//
// Created by wlwx on 2019/3/16.
//

#ifndef EASY_CHAT_EVENTLOOPTHREAD_H
#define EASY_CHAT_EVENTLOOPTHREAD_H

#include "Mutex.h"
#include "Thread.h"

class EventLoop;

class EventLoopThread : noncopyable
{
public:
    typedef  std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
            const std::string& name = std::string());

    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;

};


#endif //EASY_CHAT_EVENTLOOPTHREAD_H
