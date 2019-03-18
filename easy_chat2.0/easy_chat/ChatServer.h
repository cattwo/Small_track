//
// Created by wlwx on 2019/3/18.
//

#ifndef EASY_CHAT_CHATSERVER_H
#define EASY_CHAT_CHATSERVER_H

#include "Mutex.h"
#include "noncopyable.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include <set>
#include <stdio.h>
#include <unistd.h>

class ChatServer : noncopyable
{
public:
    ChatServer(EventLoop* loop,InetAddress& listenAddr)
            : server_(loop, listenAddr, "ChatServer")
    {
        server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&ChatServer::onStringMessage, this,std::placeholders::_1, std::placeholders::_2));
    }

    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnection::TcpConnectionPtr& conn)
    {
        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connections_.insert(conn);
        }
        else
        {
            connections_.erase(conn);
        }
    }

    void onStringMessage(const TcpConnection::TcpConnectionPtr&,const std::string& message)
    {
        MutexLockGuard lock(mutex_);
        for (ConnectionList::iterator it = connections_.begin();it != connections_.end();++it)
        {
            (*it)->send(message);
        }
    }

    typedef std::set<TcpConnection::TcpConnectionPtr> ConnectionList;
    TcpServer server_;
    MutexLock mutex_;
    ConnectionList connections_ ;
};

#endif //EASY_CHAT_CHATSERVER_H
