//
// Created by wlwx on 2019/3/15.
//

#include "Channel.h"
#include <poll.h>
#include <sys/epoll.h>
#include <sstream>
#include "EventLoop.h"

const int Channel::kNoneEvent = 0;

const int Channel::kReadEvent = POLLIN | POLLPRI;

const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
:loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1),tied_(false),
addedToLoop_(false),eventHandling_(false)
{}

Channel::~Channel()
{}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::update()
{
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent()
{
    std::shared_ptr<void> guard;
    if(tied_)
    {
        guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard();
        }
    }
}

void Channel::handleEventWithGuard()
{
    eventHandling_ = true;
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & POLLNVAL)
    {
    }

    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_) readCallback_();
    }
    if (revents_ & POLLOUT)
    {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}




