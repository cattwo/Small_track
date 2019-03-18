//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_ACCEPTOR_H
#define EASY_CHAT_ACCEPTOR_H

#include <functional>
#include "Channel.h"
#include "Socket.h"

class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
public:
    typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop,  InetAddress& listenAddr,int socketfd);

    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb;}

    bool isListening() const { return listenning_;}

    void listen();

private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listenning_;
    int idlefd_;
    NewConnectionCallback newConnectionCallback_;
};


#endif //EASY_CHAT_ACCEPTOR_H
