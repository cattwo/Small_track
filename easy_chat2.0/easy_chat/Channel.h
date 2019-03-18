//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_CHANNEL_H
#define EASY_CHAT_CHANNEL_H

#include <functional>
#include <memory>
#include "noncopyable.h"

class EventLoop;

class Channel : noncopyable
{
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);

    ~Channel();

    void handleEvent();

    void setReadCallback(const EventCallback& cb)
    {   readCallback_ = cb; }

    void setWriteCallback(const EventCallback& cb)
    {   writeCallback_ = cb; }

    void setCloseCallback(const EventCallback& cb)
    {   closeCallback = cb; }

    void setErrorCallback(const EventCallback& cb)
    {    errorCallback = cb; }

    int fd() const { return  fd_ ; }

    void tie(const std::shared_ptr<void>&);

    int events() const { return events_; }

    void set_revents(int revt) { revents_ = revt; }

    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent; update();}

    void disableReading() {events_ &= ~kReadEvent; update();}

    void enableWriting() { events_ |= kWriteEvent; update();}

    void disableWriting() { events_ &= ~kWriteEvent; update();}

    void disableAll() {events_ = kNoneEvent; update();}

    bool  isReading() { return events_ & kReadEvent;}

    bool  isWriting() { return  events_ & kWriteEvent;}

    void remove();

    int index() { return index_;}

    void set_index(int idx) {index_ = idx;}

    EventLoop* ownerLoop() { return loop_;}

private:
    void update();
    void handleEventWithGuard();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;
    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;
    std::weak_ptr<void> tie_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

};


#endif //EASY_CHAT_CHANNEL_H
