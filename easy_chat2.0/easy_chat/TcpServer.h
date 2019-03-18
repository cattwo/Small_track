//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_TCPSERVER_H
#define EASY_CHAT_TCPSERVER_H

#include "TcpConnection.h"
#include "noncopyable.h"
#include <map>

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr&,const char* ,ssize_t)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* loop, InetAddress& listenAddr,const std::string& nameArg,Option option = kNoReusePort);
    ~TcpServer();

    const std::string& ipPort() const { return ipPort_; }
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }


    void setThreadNum(int numThreads);

    void setThreadInitCallback(const ThreadInitCallback& cb) { threadInitCallback_ = cb; }

    std::shared_ptr<EventLoopThreadPool> threadPool() { return threadPool_; }

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    static int creatFdNonBlock();
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;
    const std::string ipPort_;
    const std::string name_;
    InetAddress localAddr_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ThreadInitCallback threadInitCallback_;
    bool started_;
    int nextConnId_;
    ConnectionMap connections_;

};


#endif //EASY_CHAT_TCPSERVER_H
