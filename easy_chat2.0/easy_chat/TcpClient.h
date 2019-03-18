//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_TCPCLIENT_H
#define EASY_CHAT_TCPCLIENT_H

#include "Mutex.h"
#include "TcpConnection.h"

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&,const char* ,ssize_t)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

class TcpClient : noncopyable
{
public:
    TcpClient(EventLoop* loop,const InetAddress& serverAddr,const std::string& nameArg);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        MutexLockGuard lock(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const { return loop_; }
    bool retry() const { return retry_; }
    void enableRetry() { retry_ = true; }

    const std::string& name() const
    { return name_; }

    void setConnectionCallback(ConnectionCallback cb)
    { connectionCallback_ = std::move(cb); }

    void setMessageCallback(MessageCallback cb)
    { messageCallback_ = std::move(cb); }

    void setWriteCompleteCallback(WriteCompleteCallback cb)
    { writeCompleteCallback_ = std::move(cb); }

private:

    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;
    const std::string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool retry_;
    bool connect_;
    int nextConnId_;
    mutable MutexLock mutex_;
    TcpConnectionPtr connection_ ;
};


#endif //EASY_CHAT_TCPCLIENT_H
