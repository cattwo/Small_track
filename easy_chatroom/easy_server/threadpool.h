#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "locker.h"
#include <pthread.h>
#include <cstdio>
#include <exception>
#include <queue>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <bits/socket.h>
#include <QDebug>

class Userdata
{
public:
    Userdata();
    ~Userdata();
    int sockfd;
    locker user_lock;
};

class Threadpool
{
public:
    Threadpool(int temp_thread_num = 10,int temp_request_num = 10,int temp_max_user = 50);
    ~Threadpool();
    bool append(epoll_event);/*向队列增加信息*/
    bool append_user(int);/*增加新用户*/
    static void* work(void*);
    void run();
    void handle_wrong(int);
    bool connect_listen(const char*,const char*);
    void epoll_run();
    void stop();
private:
    void addfd(int,int);
    int m_thread_number;/*线程池中线程数*/
    int m_max_requests; /*请求队列中最大请求数*/
    int m_max_user;/*最大用户数*/
    int now_user;/*当前用户数量*/
    pthread_t* m_threads; /*线程数组*/
    std::queue<epoll_event>m_workqueue;/*请求队列*/
    std::vector<Userdata>user;/*用户*/
    std::map<int,int>sockfd_user;/*sockfd与用户对应*/
    locker m_queuelocker;/*保护队列的锁*/
    locker map_locker;
    sem m_queuestat;/*队列信号量*/
    bool m_stop;/*是否结束线程*/
    struct sockaddr_in address;
    int listenfd;
    int epollfd;
    epoll_event events[1024];
};

#endif // THREADPOOL_H
