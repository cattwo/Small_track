//
// Created by wlwx on 2019/3/17.
//

#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"


TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& nameArg,
                             int sockfd,const InetAddress& localAddr,
                             const InetAddress& peerAddr)
        : loop_(loop),
          name_(nameArg),
          state_(Connecting),
          reading_(true),
          socket_(new Socket(sockfd)),
          channel_(new Channel(loop, sockfd)),
          localAddr_(localAddr),
          peerAddr_(peerAddr)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection()
{
    assert(state_ == Disconnected);
}

void TcpConnection::send(const std::string& message)
{
    if (state_ == Connected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            void (TcpConnection::*fp)(const std::string& message) = &TcpConnection::sendInLoop;
            loop_->runInLoop(std::bind(fp,this, message));
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    sendInLoop(message.c_str(), message.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    ssize_t nwrote = 0;
    if (state_ == Disconnected)
    {
        return;
    }
    if (!channel_->isWriting())
    {
        nwrote = ::write(channel_->fd(), data, len);
        if (nwrote >= 0)
        {
            if (writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
    }
}

void TcpConnection::shutdown()
{
    if (state_ == Connected)
    {
        setState(Disconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{
    if (!channel_->isWriting())
    {
        ::shutdown(channel_->fd(),SHUT_WR);
    }
}

void TcpConnection::forceClose()
{
    if (state_ == Connected || state_ == Disconnecting)
    {
        setState(Disconnecting);
        loop_->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseInLoop()
{
    if (state_ == Connected || state_ == Disconnecting)
    {
        handleClose();
    }
}

void TcpConnection::startRead()
{
    loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop()
{
    if (!reading_ || !channel_->isReading())
    {
        channel_->enableReading();
        reading_ = true;
    }
}

void TcpConnection::stopRead()
{
    loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop()
{
    if (reading_ || channel_->isReading())
    {
        channel_->disableReading();
        reading_ = false;
    }
}

void TcpConnection::connectEstablished()
{
    assert(state_ == Connecting);
    setState(Connected);
    channel_->tie(shared_from_this());
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if (state_ == Connected)
    {
        setState(Disconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead()
{
    int savedErrno = 0;
    char buf[65536];
    memset(buf,0, sizeof(buf));
    ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
    if (n > 0)
    {
        messageCallback_(shared_from_this(), buf, n);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if (channel_->isWriting())
    {
        ssize_t n = ::write(channel_->fd(),writeBuffer, sizeof(writeBuffer));
        if (n > 0)
        {
                memset(writeBuffer,0, sizeof(writeBuffer));
                channel_->disableWriting();
                if (writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == Disconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
}

void TcpConnection::handleClose()
{
    assert(state_ == Connected || state_ == Disconnecting);
    setState(Disconnected);
    channel_->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    closeCallback_(guardThis);
}

void TcpConnection::handleError()
{
    /*等日志系统实现后操作*/
}
