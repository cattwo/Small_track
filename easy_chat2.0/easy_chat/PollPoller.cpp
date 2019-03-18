//
// Created by wlwx on 2019/3/15.
//

#include "PollPoller.h"
#include "Channel.h"
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <utility>

PollPoller::PollPoller(EventLoop *loop)
:Poller(loop)
{

}

PollPoller::~PollPoller() = default;

void PollPoller::poll(int timeoutMs, Poller::ChannelList *activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    int savedErrno = errno;
    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {

    }
    else
    {
        if(savedErrno != EINTR)
            errno = savedErrno;
    }
    return;
}

void PollPoller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const
{
    for(PollFdList::const_iterator pfd = pollfds_.begin();
        pfd != pollfds_.end() && numEvents > 0; ++pfd)
    {
        if(pfd->revents > 0)
        {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            Channel* channel = ch->second;
            assert(channel->fd() == pfd->fd);
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::updateChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    if(channel->index() < 0)
    {
        assert(channels_.find(channel->fd()) == channels_.end());
        struct  pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short >(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size()) - 1;
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    }
    else
    {
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int idx = channel->index();
        assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
        struct  pollfd& pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd() - 1);
        pfd.fd = channel->fd();
        pfd.events = static_cast<short >(channel->events());
        pfd.revents = 0;
        if(channel->isNoneEvent())
        {
            pfd.fd = -channel->fd() - 1;
        }
    }
}

void PollPoller::removeChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
    const struct  pollfd pfd = pollfds_[idx];
    assert(pfd.fd == -channel->fd() - 1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());
    assert(n == 1);
    if(static_cast<size_t >(idx) == pollfds_.size() - 1)
    {
        pollfds_.pop_back();
    } else
    {
        /*此处可优化，交换后删除最后，而不是erase*/
        int channelAtEnd = pollfds_.back().fd;
        iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
        if(channelAtEnd < 0)
        {
            channelAtEnd = - channelAtEnd - 1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}



