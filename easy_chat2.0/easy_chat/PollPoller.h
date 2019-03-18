//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_POLLPOLLER_H
#define EASY_CHAT_POLLPOLLER_H

#include "Poller.h"
#include <vector>

struct  pollfd;

class PollPoller : public Poller
{
public:
    PollPoller(EventLoop* loop);

    ~PollPoller() override;

    void poll(int timeoutMs, ChannelList* activeChannels) override;

    void updateChannel(Channel* channel) override;

    void removeChannel(Channel* channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> PollFdList;

    PollFdList pollfds_;

};


#endif //EASY_CHAT_POLLPOLLER_H
