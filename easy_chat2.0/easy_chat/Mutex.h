//
// Created by wlwx on 2019/3/15.
//

#ifndef EASY_CHAT_MUTEX_H
#define EASY_CHAT_MUTEX_H

#include <assert.h>
#include <pthread.h>
#include "noncopyable.h"

class MutexLock : noncopyable
{
public:
    MutexLock()
    :holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }

//    bool isLockedByThisThread() const
//    {
//        return true;
//    }

void lock()
{
        pthread_mutex_lock(&mutex_);
        //
}

void unlock()
{
        //
        pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* getPthreadMutex()
{
        return &mutex_;
}

private:

    pid_t  holder_;
    pthread_mutex_t mutex_;

};

class  MutexLockGuard : noncopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex)
    :mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

class Condition : noncopyable
{
public:
    explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};

#endif //EASY_CHAT_MUTEX_H
