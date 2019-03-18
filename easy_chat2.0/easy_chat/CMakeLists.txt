cmake_minimum_required(VERSION 3.13)
project(easy_chat)

set(CMAKE_CXX_STANDARD 11)

add_executable(easy_chat noncopyable.h Mutex.h EventLoop.h Channel.cpp Channel.h Poller.cpp Poller.h PollPoller.cpp PollPoller.h EPollPoller.cpp EPollPoller.h EventLoop.cpp EventLoopThread.cpp EventLoopThread.h Thread.cpp Thread.h EventLoopThreadPool.cpp EventLoopThreadPool.h InetAddress.cpp InetAddress.h Socket.cpp Socket.h Acceptor.cpp Acceptor.h TcpServer.cpp TcpServer.h TcpConnection.cpp TcpConnection.h Buffer.cpp Buffer.h TcpClient.cpp TcpClient.h Connector.cpp Connector.h ThreadLocalSingleton.h ChatServer.cpp ChatServer.h ChatClient.cpp ChatClient.h)