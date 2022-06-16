//
// Created by gsr on 2022/4/29.
//

//#ifndef CWEBSERVER_THREADPOOL_H
//#define CWEBSERVER_THREADPOOL_H

#pragma once

#include<iostream>
#include<list>
#include<cstdio>
#include<semaphore.h>
#include<exception>
#include<pthread.h>
#include <memory>
#include <functional>
#include <vector>

#include"lock.h"


const int MAX_THREAD_SIZE = 1024;   // 线程数上限
const int MAX_QUEUE_SIZE = 10000;   // 最大队上限

struct ThreadTask {                                     // 线程任务结构体
    std::function<void(std::shared_ptr<void>)> process; // 封装一个函数对象，作为所需执行任务的指代，封装的函数指针
    std::shared_ptr<void> arg;                          // 这里是任务的参数，无类型智能指针
};

/*线程池的封装*/
class threadpool
{
private:
    bool m_stop;                         //是否结束线程
    int thread_size;                     //线程数
    int max_queue_size;                  //最大等待队列数
    std::vector<pthread_t> threads;      //线程池数组
    std::list<ThreadTask> request_queue; //请求队列

    mutex_locker mutex_;                 //保护请求队列的互斥锁
    cond_locker condition_;              //条件变量
    sem m_queuestat;                     //由信号量来判断是否有任务需要处理

public:
    threadpool(int thread_s, int max_queue_s);
    ~threadpool();

    bool addjob(std::shared_ptr<void> arg, std::function<void(std::shared_ptr<void>)> fun);

    void shutdown();

private:
    static void* worker(void *arg);

    void run();
};



//#endif //CWEBSERVER_THREADPOOL_H
