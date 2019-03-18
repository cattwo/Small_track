//
// Created by wlwx on 2019/3/15.
//

#include "EPollPoller.h"
#include "Channel.h"
#include <assert.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <memory.h>

static_assert(EPOLLIN == POLLIN);
static_assert(EPOLLPRI == POLLPRI);
static_assert(EPOLLOUT == POLLOUT);
static_assert(EPOLLRDHUP == POLLRDHUP);
static_assert(EPOLLERR == POLLERR);
static_assert(EPOLLHUP == POLLHUP);

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
:Poller(loop),
epollfd_(::epoll_create1(EPOLL_CLOEXEC)).
events_(kInitEventListSize)
{
    //
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

void EPollPoller::poll(int timeoutMs, Poller::ChannelList *activeChannels)
{
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()),timeoutMs);
    int saveErrno = errno;
    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t >(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0)
    {
        //
    } else
    {
        if(saveErrno != EINTR)
            errno = saveErrno;
    }
    return;
}

void EPollPoller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const
{
    assert(static_cast<size_t >(numEvents) <= events_.size());
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
#endif
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::updateChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    //index的值用来标记此channel是否在epoll的关注列表之中
    //-1 表示新的，2表示已经删除的，均需要ADD
    const int index = channel->index();
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }else
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        //1表示已经添加的，需要删除或者修改
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0,sizeof event);
    event.events = channel->events();
    //利用epoll_data的ptr指针来存放channel
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        //
    }
}

void EPollPoller::removeChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    assert(n == 1);
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}


