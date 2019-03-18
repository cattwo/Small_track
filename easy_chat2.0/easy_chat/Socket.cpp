//
// Created by wlwx on 2019/3/17.
//

#include "Socket.h"
#include "InetAddress.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

Socket::~Socket()
{
    ::close(sockfd_);
}

void Socket::bindAddress(InetAddress* localAddr)
{
    ::bind(sockfd_, (sockaddr*)localAddr->getAddr(), sizeof(localAddr->getAddr()));
}

void Socket::listen()
{
    ::listen(sockfd_, backlog_);
}

int Socket::accept(InetAddress *peerAddr)
{
    socklen_t addrlen = sizeof(*peerAddr->getAddr());
    int connfd_ = ::accept4(sockfd_,(sockaddr*)peerAddr->getAddr(),&addrlen,SOCK_CLOEXEC | SOCK_NONBLOCK);
    return connfd_;
}