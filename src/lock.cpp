//
// Created by gsr on 2022/4/25.
//

#include "../include/lock.h"

sem :: sem()
{
    if(sem_init(&m_sem,0,0) != 0)
    {
        throw std ::exception();
    }
}

sem :: ~sem()
{
    sem_destroy(&m_sem);
}

bool sem::wait()
{
    return sem_wait(&m_sem) == 0;
}

bool sem::post()
{
    return sem_post(&m_sem) == 0;
}

/*------------------------------------------------------------------*/

mutex_locker::mutex_locker()
{
    if(pthread_mutex_init(&m_mutex, NULL) != 0)
    {
        throw std::exception();
    }
}

mutex_locker::~mutex_locker()
{
    pthread_mutex_destroy(&m_mutex);
}

bool mutex_locker::lock()
{
    return pthread_mutex_lock(&m_mutex) == 0;
}

bool mutex_locker::unlock()
{
    return pthread_mutex_unlock(&m_mutex) == 0;
}

pthread_mutex_t * mutex_locker::get_mutex(){
    return &m_mutex;
}


/*------------------------------------------------------------------*/

cond_locker::cond_locker(mutex_locker &mutex) : m_mutex(mutex)
{
    if(pthread_cond_init(&m_cond, NULL) != 0)
    {
        throw std::exception();
    }
}

cond_locker::~cond_locker()
{

    pthread_cond_destroy(&m_cond);
}

bool cond_locker::wait()
{
    return pthread_cond_wait(&m_cond, m_mutex.get_mutex()) == 0;
}

bool cond_locker::signal()
{
    return pthread_cond_signal(&m_cond) == 0;
}

bool cond_locker::signal_all()
{
    return pthread_cond_broadcast(&m_cond) == 0;
}
