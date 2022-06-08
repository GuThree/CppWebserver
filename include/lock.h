//
// Created by gsr on 2022/4/25.
//

//#ifndef CWEBSERVER_LOCK_H
//#define CWEBSERVER_LOCK_H

#pragma once

#include<iostream>
#include<list>
#include<cstdio>
#include<semaphore.h>
#include<exception>
#include<pthread.h>

/*封装信号量*/
class sem{
private:
    sem_t m_sem;
public:
    sem();
    ~sem();
    bool wait();//等待信号量
    bool post();//增加信号量
};

/*封装互斥锁*/
class mutex_locker{
private:
    pthread_mutex_t m_mutex;
public:
    mutex_locker();
    ~mutex_locker();
    bool lock();
    bool unlock();
    pthread_mutex_t *get_mutex();
};

/*封装条件变量*/
class cond_locker{
private:
    mutex_locker &m_mutex;
    pthread_cond_t m_cond;
public:
    cond_locker(mutex_locker &mutex);
    ~cond_locker();
    bool wait();
    bool signal();
    bool signal_all();
};

//#endif //CWEBSERVER_LOCK_H
