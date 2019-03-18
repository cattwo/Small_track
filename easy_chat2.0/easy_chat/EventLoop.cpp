//
// Created by wlwx on 2019/3/16.
//

#include "EventLoop.h"
#include <algorithm>
#include "Channel.h"
#include "Poller.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>
#include "PollPoller.h"
#include "EPollPoller.h"

/*线程全局变量*/
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

int createEventfd()
{
    /*创建事件对象，实现线程间通信*/
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        //
        abort();
    }
    return evtfd;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

EventLoop::EventLoop()
:looping_(false),quit_(false),threadId_(0),
poller_(new EPollPoller(this)),wakeupFd_(createEventfd()),
eventHandling_(false),currentActiveChannel_(NULL)
{
    if(! t_loopInThisThread)
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    looping_ = true;
    quit_ = false;
    while (!quit_)
    {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        eventHandling_ = true;
        for (Channel* channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent();
        }
        eventHandling_ = false;
        currentActiveChannel_ = NULL;
        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }
}

size_t EventLoop::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return pendingFunctors_.size();
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    if (eventHandling_)
    {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (const Functor& functor : functors)
    {
        functor();
    }
}
