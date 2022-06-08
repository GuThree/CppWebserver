//
// Created by gsr on 2022/5/25.
//

#include "../include/timer.h"
#include "../include/epoll.h"

size_t TimerNode::current_millisecond_time = 0; // 初始化

TimerNode::TimerNode(std::shared_ptr<HttpData> httpData, size_t timeout) : deleted_(false), httpData(httpData) {
    current_time();
    expiredTime = current_millisecond_time + timeout;
}

TimerNode::~TimerNode() {
    // 析构时如果是被deleted 则httpData为NULL, 不用处理，而如果是超时，则需要删除Epoll中的httpDataMap中的指向对应HttpData的指针，释放资源
    if (httpData) {
        auto it = Epoll::httpDataMap.find(httpData->clientSocket_->m_sockfd);
        if (it != Epoll::httpDataMap.end()) {
            Epoll::httpDataMap.erase(it);
        }
    }
}

void inline TimerNode::current_time() {
    struct timeval cur;
    gettimeofday(&cur, NULL);
    current_millisecond_time = (cur.tv_sec * 1000) + (cur.tv_usec / 1000);  // 设置当前时间，以毫秒为单位
}

void TimerNode::deleted() {
    // 删除采用标记，并及时析构HttpData，以关闭描述符
    // 关闭定时器时应该把 httpDataMap 里的HttpData 一起erase
    httpData.reset();   //释放智能指针中内置的指针指向的空间,指针还在
    deleted_ = true;
}

void TimerManager::addTimer(std::shared_ptr<HttpData> httpData, size_t timeout) {
    Shared_TimerNode timerNode(new TimerNode(httpData, timeout));
    {
        this->lock_.lock();
        this->TimerQueue.push(timerNode);
        // 将TimerNode和HttpData关联起来
        httpData->setTimer(timerNode);
        this->lock_.unlock();
    }
}

// 处理计时器队列中的超时计时器
void TimerManager::handle_expired_event() {
    this->lock_.lock();
    // 更新当前时间
    TimerNode::current_time();
    while(!TimerQueue.empty()) {
        Shared_TimerNode timerNode = TimerQueue.top();
        if (timerNode->isDeleted()) {
            // 删除节点
            TimerQueue.pop();
        } else if (timerNode->isExpired()) {
            // 过期删除
            TimerQueue.pop();
        } else {    // 如果这个计时器没有到时而且不是deleted的状态，则队列后面的也不会是这一种状态
            break;
        }
    }
    this->lock_.unlock();
}