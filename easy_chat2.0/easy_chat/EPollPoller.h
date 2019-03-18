//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_EPOLLPOLLER_H
#define EASY_CHAT_EPOLLPOLLER_H

#include <vector>
#include "Poller.h"

struct epoll_event;

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);

    ~EPollPoller() override;

    void poll(int timeoutMs, ChannelList* activeChannels) override;

    void updateChannel(Channel* channel) override;

    void removeChannel(Channel* channel) override;

private:
    static const  int kInitEventListSize = 16;

    //用于记录日志，暂无实现 static const char* operationToString(int op);

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const ;

    void update(int operation, Channel* channel);

    typedef std::vector<struct  epoll_event> EventList;

    int epollfd_;

    EventList events_;

};


#endif //EASY_CHAT_EPOLLPOLLER_H
