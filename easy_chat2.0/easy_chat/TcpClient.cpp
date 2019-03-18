//
// Created by wlwx on 2019/3/17.
//

#include "TcpClient.h"
#include "Connector.h"
#include "EventLoop.h"
#include <stdio.h>



TcpClient::TcpClient(EventLoop* loop,const InetAddress& serverAddr,const std::string& nameArg)
        : loop_(loop),connector_(new Connector(loop, serverAddr)),name_(nameArg),connectionCallback_(NULL),messageCallback_(NULL),retry_(false),connect_(true),nextConnId_(1)
{
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this,std::placeholders:: _1));
}

TcpClient::~TcpClient()
{
    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn)
    {
        assert(loop_ == conn->getLoop());
        loop_->quit();
        if (unique)
        {
            conn->forceClose();
        }
    }
    else
    {
        connector_->stop();
    }
}

void TcpClient::connect()
{
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()
{
    connect_ = false;

    {
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    InetAddress peerAddr;
    InetAddress localAddr;
    TcpConnectionPtr conn(new TcpConnection(loop_,"",sockfd,localAddr,peerAddr));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    assert(loop_ == conn->getLoop());

    {
        MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_)
    {
        connector_->restart();
    }
}