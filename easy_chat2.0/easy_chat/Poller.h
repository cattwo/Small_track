//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_POLLER_H
#define EASY_CHAT_POLLER_H

#include <map>
#include <vector>
#include "EventLoop.h"

class Channel;

class Poller : noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);

    virtual ~Poller();

    virtual void updateChannel(Channel* channel) = 0;

    virtual void removeChannel(Channel* channel) = 0;

    virtual  bool hasChannel(Channel* channel) const ;

    virtual void poll(int timeoutMs, ChannelList* activeChannels) = 0;

    void assertInLoopThread() const
    {
        //
    }

    //static Poller* newPoller(EventLoop* loop,bool type);

protected:
    typedef  std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;
};


#endif //EASY_CHAT_POLLER_H
