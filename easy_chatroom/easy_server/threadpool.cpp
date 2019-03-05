#include "threadpool.h"

Userdata::Userdata()
    :sockfd(-1)
{

}

Userdata::~Userdata()
{

}

Threadpool::Threadpool(int temp_thread_num,int temp_request_num,int temp_max_user)
    :m_thread_number(temp_thread_num),
     m_max_requests(temp_request_num),
     m_max_user(temp_max_user),
     now_user(0),m_stop(false),m_threads(nullptr),listenfd(-1),epollfd(-1)
{
    if((m_thread_number <= 0) || (m_max_requests <= 0) || (m_max_user <= 0))
    {
        //throw std::exception();
        qDebug()<<m_thread_number<<" "<<m_max_requests<<" "<<m_max_user;
    }

    m_threads = new pthread_t[m_thread_number];
    if(!m_threads)
    {
        throw std::exception();
    }

    user = std::vector<Userdata>(m_max_user);

    for(int i = 0; i < m_thread_number; i++)
    {
        if(pthread_create(m_threads + i, nullptr, work, this) != 0)
        {
            break;
        }
        if(pthread_detach(m_threads[i]))
        {

        }
    }

}

Threadpool::~Threadpool()
{
    delete[] m_threads;
    m_stop = true;
}

bool Threadpool::append(epoll_event temp_event)
{
    m_queuelocker.lock();
    if(m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push(temp_event);
    m_queuelocker.unlock();
    m_queuestat.post();
    qDebug()<<"post\n";
    return true;
}

void* Threadpool::work(void* arg)
{
    Threadpool* pool = (Threadpool*)arg;
    pool->run();
    return pool;
}

bool Threadpool::connect_listen(const char* temp_ip, const char* temp_port)
{
    const char* ip = temp_ip;
    int port = atoi(temp_port);
    int ret = 0;
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        return false;
    }
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1)
    {
        return false;
    }
    ret = listen(listenfd,10);
    if(ret == -1)
    {
        return  false;
    }
    return true;
}

void Threadpool::addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd, &event);
    qDebug()<<"addfd\n";
    /*此处未设置fd非阻塞*/
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
}

void Threadpool::epoll_run()
{
    epollfd = epoll_create(50);
    if(epollfd == -1)
    {
        throw std::exception();
    }
    epoll_event listen_event;
    listen_event.data.fd = listenfd;
    listen_event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd, &listen_event);
    qDebug()<<"epoll_listen\n";
    while (!m_stop)
    {
        int number = epoll_wait(epollfd,events,1024, -1);
        if(number < 0)
        {
            qDebug()<<"epoll_wait wrong";
            break;
        }
        for(int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd)
            {
                qDebug()<<"epoll_comeon"<<number<<"\n";
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof (client_address);
                int connfd = accept(listenfd, (struct sockaddr*)&client_address,&client_addrlength);
                if(now_user < m_max_user)
                {
                    map_locker.lock();
                    //user[now_user].user_lock.lock();
                    addfd(epollfd,connfd);                    
                    user[now_user].sockfd = connfd;
                    qDebug()<<"connfd and nowuser"<<now_user<<" "<<connfd<<"\n";
                    sockfd_user[connfd] = now_user;
                    ++now_user;
                    //user[now_user].user_lock.unlock();
                    map_locker.unlock();
                }
            }
            else
            {
                append(events[i]);
            }
        }

    }
    close(listenfd);
    return;
}

void Threadpool::handle_wrong(int id)
{
    qDebug()<<"close1\n";
    if(id != now_user - 1)
    {
        --now_user;
        user[now_user].user_lock.lock();
        epoll_ctl(epollfd,EPOLL_CTL_DEL,user[id].sockfd,nullptr);
        close(user[id].sockfd);
        sockfd_user[user[id].sockfd] = -1;
        user[id].sockfd = user[now_user].sockfd;
        sockfd_user[user[id].sockfd] = id;
        user[now_user].sockfd = -1;
        user[now_user].user_lock.unlock();
    }
    else if(id >= 0 && id == now_user - 1)
    {
        --now_user;
        epoll_ctl(epollfd,EPOLL_CTL_DEL,user[id].sockfd,nullptr);
        close(user[id].sockfd);
        sockfd_user[user[id].sockfd] = -1;
        user[now_user].sockfd = -1;
    }
}

void Threadpool::run()
{
    char data[1024];
    while(!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        epoll_event event = m_workqueue.front();
        m_workqueue.pop();
        m_queuelocker.unlock();
        int id = sockfd_user[event.data.fd];
        memset(data,0,sizeof (data));
        qDebug()<<"now in thread\n";
        if(event.events & EPOLLIN)
        {            
            user[id].user_lock.lock();
            //qDebug()<<id<<" "<<user[id].sockfd<<"\n";
            int ret = recv(user[id].sockfd,data,sizeof (data) - 1,0);
            //qDebug()<<"recv \n";
            if(ret <= 0)
            {
                handle_wrong(id);
                user[id].user_lock.unlock();
                continue;
            }
            qDebug()<<"recv "<<ret<<"\n";
            user[id].user_lock.unlock();
            for(int i = 0; i < m_max_user; i++)
            {
                if(i != id)
                {
                    user[i].user_lock.lock();
                    if(user[i].sockfd == -1)
                    {
                        user[i].user_lock.unlock();
                        break;
                    }
                    qDebug()<<"send"<<i<<" "<<user[i].sockfd<<"\n";
                    ret = send(user[i].sockfd,data,sizeof (data),0);
                    if(ret <= 0)
                    {
                        handle_wrong(id);
                        user[i].user_lock.unlock();
                        continue;
                    }
                    user[i].user_lock.unlock();
                }
            }
        }
        else
        {
            qDebug()<<"something wrong\n";

            handle_wrong(id);
        }
    }
}

void Threadpool::stop()
{
    m_stop = true;
}










