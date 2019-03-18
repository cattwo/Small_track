//
// Created by wlwx on 2019/3/18.
//

#include "Connector.h"
#include "Channel.h"
#include "EventLoop.h"
#include "errno.h"
#include <sys/types.h>
#include <sys/socket.h>



const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
        : loop_(loop),serverAddr_(serverAddr),
          connect_(false),state_(kDisconnected),
          retryDelayMs_(kInitRetryDelayMs)
{}

Connector::~Connector()
{
    assert(!channel_);
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    assert(state_ == kDisconnected);
    if (connect_)
    {
        connect();
    }
    else
    {
    }
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
    if (state_ == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect()
{
    int sockfd = ::socket(PF_INET, SOCK_STREAM |SOCK_NONBLOCK |SOCK_CLOEXEC,0);
    int ret = ::connect(sockfd,(sockaddr*)serverAddr_.getAddr(), sizeof(serverAddr_.getAddr()));
}

void Connector::restart()
{
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));
    channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{

}

void Connector::handleError()
{
}
