//
// Created by wlwx on 2019/3/16.
//

#include "Thread.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <assert.h>

pid_t  gettid()
{
    return static_cast<pid_t >(::syscall(SYS_getpid));
}

struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;

    ThreadData(ThreadFunc func):func_(std::move(func))
    {}

    void runInThread()
    {
        func_();
    }

};

void* startThread(void* arg)
{
    ThreadData* temp = static_cast<ThreadData*>(arg);
    temp->runInThread();
    delete temp;
    return NULL;
}

Thread::Thread(Thread::ThreadFunc func, const std::string &name)
:started_(false), joined_(false),
pthreadId_(0),tid_(0),
func_(std::move(func)),name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_);
    if(pthread_create(&pthreadId_, NULL, &startThread, data))
    {
        started_ = false;
        delete data;
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}