//
// Created by wlwx on 2019/3/17.
//

#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

Acceptor::Acceptor(EventLoop *loop,  InetAddress &listenAddr,int socketfd)
:loop_(loop),acceptSocket_(socketfd),
acceptChannel_(loop,acceptSocket_.fd()),
listenning_(false)
{
    acceptSocket_.bindAddress(&listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if(newConnectionCallback_)
    {
        newConnectionCallback_(connfd,peerAddr);
    }
    else
    {
        ::close(connfd);
    }
}
