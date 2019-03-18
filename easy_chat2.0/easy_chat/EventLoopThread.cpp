//
// Created by wlwx on 2019/3/16.
//

#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const std::string& name)
        : loop_(NULL),
          exiting_(false),
          thread_(std::bind(&EventLoopThread::threadFunc, this), name),
          mutex_(),
          cond_(mutex_),
          callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());
    /*线程开始，注册函数为threadFunc*/
    thread_.start();

    EventLoop* loop = NULL;
    {
        MutexLockGuard lock(mutex_);
        while (loop_ == NULL)
        {
            cond_.wait();
        }
        loop = loop_;
    }

    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    if (callback_)
    {
        callback_(&loop);
    }

    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();

    /*销毁时也要加锁*/
    MutexLockGuard lock(mutex_);
    loop_ = NULL;
}