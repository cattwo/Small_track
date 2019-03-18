//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_TCPCONNECTION_H
#define EASY_CHAT_TCPCONNECTION_H

#include <functional>
#include "noncopyable.h"
#include "Buffer.h"
#include "InetAddress.h"
#include <memory>

class Channel;
class EventLoop;
class Socket;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr&,const char* ,ssize_t)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

    TcpConnection(EventLoop* loop, const std::string& name,
                                int sockfd, const InetAddress& localAddr,
                                const InetAddress& peerAddr);

    ~TcpConnection();

    EventLoop* getLoop() const { return loop_;}

    const std::string& name() const { return name_;}

    const InetAddress& localAddress() const { return localAddr_; }

    const InetAddress& peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == Connected; }

    bool disconnected() const { return state_ == Disconnected; }

    //暂时只支持string
    //void send(const void* message, int len);
    void send(const std::string& message);

    void shutdown();

    void forceClose();

    void forceCloseWithDelay(double seconds);

    void setTcpNoDelay(bool on);

    void startRead();

    void startReadInLoop();

    void stopRead();

    void stopReadInLoop();

    bool isReading() const { return reading_; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; };


    void connectEstablished();

    void connectDestroyed();


private:
    enum StateE { Disconnected, Connecting, Connected, Disconnecting};
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const std::string& message);
    void sendInLoop(const void* data, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    void setState(StateE s) { state_ = s;}

    EventLoop* loop_;
    const std::string name_;
    StateE state_;
    bool reading_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;
    char writeBuffer[65536];
    /*留待日后完善*/
//    Buffer inputBuffer_;
//    Buffer outputBuffer_;

};




#endif //EASY_CHAT_TCPCONNECTION_H
