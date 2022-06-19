//
// Created by gsr on 2022/4/29.
//

#include "../include/threadpool.h"
#include "../include/logger.h"

/*线程池的创建*/
threadpool::threadpool(int thread_s, int max_queue_s) : thread_size(thread_s), max_queue_size(max_queue_s), condition_(mutex_) {
    m_stop = false;
    if (thread_s <= 0 || thread_s > MAX_THREAD_SIZE) {  // 排除异常情况
        thread_size = MAX_THREAD_SIZE;
    }
    if (max_queue_s <= 0 || max_queue_s > MAX_QUEUE_SIZE) {
        max_queue_size = MAX_QUEUE_SIZE;
    }
    threads.resize(thread_size);

    for(int i=0; i < thread_size; i++) {
        if(pthread_create(&threads[i], NULL, worker, this) != 0) {
            LogFile::writeInfo("ThreadPool init error\n");
//            std::cout << "ThreadPool init error" << std::endl;
            throw std::exception();
        }
        if(pthread_detach(threads[i])) {    //将线程分离
            throw std::exception();
        }
    }
}

threadpool::~threadpool() {}

bool threadpool::addjob(std::shared_ptr<void> arg, std::function<void (std::shared_ptr<void>)> fun)
{
    if (m_stop) {    // 如果已经关闭线程池
        LogFile::writeInfo("ThreadPool has shutdown\n");
//        std::cout << "ThreadPool has shutdown" << std::endl;
        return false;
    }

    mutex_.lock();
    if(request_queue.size() > max_queue_size)
    {
        mutex_.unlock();
        return false;
    }
    ThreadTask threadTask;
    threadTask.arg = arg;
    threadTask.process = fun;

    request_queue.push_back(threadTask);    //将请求加入到请求队列中
    mutex_.unlock();
    m_queuestat.post();
    condition_.signal();
    return true;
}

void threadpool::shutdown(){
    mutex_.lock();
    if (m_stop) {
//        std::cout << "has shutdown" << std::endl;
        LogFile::writeInfo("has shutdown\n");
    }
    for (int i = 0; i < thread_size; i++) {
        if (pthread_join(threads[i], NULL) != 0) {  // 一个个回收线程资源
            LogFile::writeInfo("pthread_join error\n");
//            std::cout << "pthread_join error" <<std::endl;
        }
    }
    m_stop = true;
    mutex_.unlock();
}

void* threadpool::worker(void *arg)
{
    threadpool *pool = static_cast<threadpool *>(arg);
    if (pool == NULL)
        return NULL;
    pool->run();
    return NULL;
}

void threadpool::run()
{
    while(true) {
        ThreadTask  requestTask;

        m_queuestat.wait();
        mutex_.lock();
        while(request_queue.empty() && !m_stop) {
            condition_.wait();  //等待被唤醒
        }
        if(m_stop) {
            break;
        }
        requestTask = request_queue.front();
        request_queue.pop_front();
        mutex_.unlock();
        requestTask.process(requestTask.arg);
    }
}