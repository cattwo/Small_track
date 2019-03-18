//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_EVENTLOOP_H
#define EASY_CHAT_EVENTLOOP_H

#include <algorithm>
#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <functional>
#include "Mutex.h"

class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();

    ~EventLoop();

    void loop();

    void quit();

    void runInLoop(Functor cb);

    void queueInLoop(Functor cb);

    size_t  queueSize() const;

    void wakeup();

    void updateChannel(Channel* channel);

    void removeChannel(Channel* channel);

    bool  hasChannel(Channel* channel);

    bool isInLoopThread() const { return true;}

    bool eventHandling() const { return eventHandling_; }

    static EventLoop* getEventLoopOfCurrentThread();




private:

    void handleRead();
    void doPendingFunctors();
    typedef  std::vector<Channel*> ChannelList;
    bool looping_;
    bool quit_;
    const pid_t  threadId_;
    int wakeupFd_;
    bool eventHandling_;

    std::unique_ptr<Poller> poller_;
    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;
    Channel* currentActiveChannel_;
    std::vector<Functor> pendingFunctors_;


    mutable MutexLock mutex_;

};

#endif //EASY_CHAT_EVENTLOOP_H
