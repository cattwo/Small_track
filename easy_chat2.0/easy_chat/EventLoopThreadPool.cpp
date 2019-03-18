//
// Created by wlwx on 2019/3/17.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <stdio.h>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
        : baseLoop_(baseLoop),
          name_(name),
          started_(false),
          numThreads_(0),
          next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool(){}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    assert(!started_);
    started_ = true;
    for (int i = 0; i < numThreads_; ++i)
    {
        /*创建新线程循环，并保存*/
        EventLoopThread* t = new EventLoopThread(cb, "");
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
    if (numThreads_ == 0 && cb)
    {
        cb(baseLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(started_);
    EventLoop* loop = baseLoop_;
    if (!loops_.empty())
    {
        /*负载均衡，每次有新连接，会分配到下一个loop，避免分配不均*/
        loop = loops_[next_];
        ++next_;
        if (static_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    assert(started_);
    if (loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}
