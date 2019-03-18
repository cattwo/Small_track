//
// Created by wlwx on 2019/3/17.
//

#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int TcpServer::creatFdNonBlock()
{
    return ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
}


TcpServer::TcpServer(EventLoop* loop, InetAddress& listenAddr,const std::string& nameArg,Option option)
        : loop_(loop),
          ipPort_(""),
          name_(""),
          localAddr_(listenAddr),
          acceptor_(new Acceptor(loop, listenAddr,TcpServer::creatFdNonBlock())),
          threadPool_(new EventLoopThreadPool(loop, name_)),
          connectionCallback_(NULL),
          messageCallback_(NULL),
          nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1,std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (auto& item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
        threadPool_->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen, *acceptor_));
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    EventLoop* ioLoop = threadPool_->getNextLoop();
    ++nextConnId_;
    std::string connName = name_ + std::to_string(nextConnId_);
    TcpConnectionPtr conn(new TcpConnection(ioLoop,connName,sockfd,localAddr_,peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this,std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    size_t n = connections_.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
