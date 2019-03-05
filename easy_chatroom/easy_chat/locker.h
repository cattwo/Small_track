#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

/*封装信号量*/
class sem
{
public:
    /*创建并初始化信号量*/
    sem()
    {
        /*第二个参数控制进程共享，第三个参数初始化信号量*/
        if(sem_init(&m_sem,0,0) != 0)
        {
            throw std::exception();
        }
    }
    /*销毁信号量*/
    ~sem()
    {
        sem_destroy(&m_sem);
    }
    /*等待信号量*/
    bool wait()
    {
        return sem_wait(&m_sem) == 0;
    }
    /*增加信号量*/
    bool post()
    {
        return sem_post(&m_sem) == 0;
    }
private:
    sem_t m_sem;
};

/*封装互斥锁*/
class locker
{
public:
    /*创建和初始化互斥锁*/
    locker()
    {
        /*第二个参数默认互斥类属性*/
        if(pthread_mutex_init(&m_mutex,nullptr) != 0)
        {
            throw  std::exception();
        }
    }
    /*销毁互斥锁*/
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    /*获取锁*/
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    /*获取锁不阻塞*/
    bool try_lock()
    {
        return pthread_mutex_trylock(&m_mutex) == 0;
    }
    /*释放锁*/
    bool unlock()
    {
        return  pthread_mutex_unlock(&m_mutex) == 0;
    }
private:
    pthread_mutex_t m_mutex;
};

/*封装条件变量*/
class cond
{
public:
    /*创建并初始化条件变量，需要先初始化锁*/
    cond()
    {
        if(pthread_mutex_init(&m_mutex,nullptr) != 0)
        {
            throw std::exception();
        }
        /*第二个参数为属性*/
        if(pthread_cond_init(&m_cond,nullptr) != 0)
        {
            /*出错应销毁已分配的资源*/
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    /*销毁条件变量*/
    ~cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    /*等待条件变量*/
    bool wait()
    {
        /*锁用来保证原子性操作，故应先加锁*/
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond,&m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    /*唤醒等待条件变量的线程*/
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    /*广播唤醒*/
    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif // LOCKER_H
