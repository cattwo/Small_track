//
// Created by wlwx on 2019/3/18.
//

#ifndef EASY_CHAT_CHATCLIENT_H
#define EASY_CHAT_CHATCLIENT_H

#include "noncopyable.h"
#include "TcpClient.h"
#include "Mutex.h"
#include "EventLoopThread.h"


class ChatClient : noncopyable
{
public:
    ChatClient(EventLoop* loop, const InetAddress& serverAddr)
            : client_(loop, serverAddr, "ChatClient")
    {
        client_.setConnectionCallback(std::bind(&ChatClient::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(std::bind(&ChatClient::onRead,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
        client_.enableRetry();
    }

    void onRead(const TcpConnectionPtr, const char* message, ssize_t ssize)
    {
        printf("<<:: %s \n",message);
    }

    void connect()
    {
        client_.connect();
    }

    void disconnect()
    {
        client_.disconnect();
    }

    void write(const std::string& message)
    {
        MutexLockGuard lock(mutex_);
        if (connection_)
        {
            connection_->send(message);
        }
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onStringMessage(const TcpConnectionPtr&,const std::string& message)
    {
        printf("<<< %s\n", message.c_str());
    }

    TcpClient client_;
    MutexLock mutex_;
    TcpConnectionPtr connection_ ;
};

#endif //EASY_CHAT_CHATCLIENT_H
