//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_SOCKET_H
#define EASY_CHAT_SOCKET_H

#include "noncopyable.h"

class InetAddress;

class Socket : noncopyable
{
public:
    explicit Socket(int sockfd)
    :sockfd_(sockfd),backlog_(100)
    {}

    ~Socket();

    int fd() const { return sockfd_;}

    void bindAddress(InetAddress* localAddr);

    void listen();

    void setbacklog(int backlog) { backlog_ = backlog;}

    int accept(InetAddress* peerAddr);

private:
    const int sockfd_;
    int backlog_;

};



#endif //EASY_CHAT_SOCKET_H
