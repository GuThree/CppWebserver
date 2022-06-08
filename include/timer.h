//
// Created by gsr on 2022/5/25.
//

//#ifndef CWEBSERVER_TIMER_H
//#define CWEBSERVER_TIMER_H

#pragma once

#include <sys/time.h>
#include <netinet/in.h>
#include <deque>
#include <queue>
#include <vector>
#include "http_data.h"
#include "lock.h"
#include <memory>

class HttpData;

//定时器类
class TimerNode
{
public:
    TimerNode(std::shared_ptr<HttpData> httpData, size_t timeout);//参数timeout为生命周期
    ~TimerNode();

    bool isDeleted() const { return deleted_; }

    bool isExpired() {
        return expiredTime < current_millisecond_time;
    }

    size_t getExpireTime() { return expiredTime; }

    std::shared_ptr<HttpData> getHttpData() { return httpData; }

    void deleted();

    static void current_time();                             //获取当前时间

    static size_t current_millisecond_time;                 //当前运行的时间数，以毫秒为单位

private:
    bool deleted_;                              //是否取消
    size_t expiredTime;                         //生命结束的时间点
    std::shared_ptr<HttpData> httpData;         //报文对象
};

struct TimerCmp {               //从小到大
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const {
        return a->getExpireTime() > b->getExpireTime();
    }
};

class TimerManager {
public:
    typedef std::shared_ptr<TimerNode> Shared_TimerNode;

    void addTimer(std::shared_ptr<HttpData> httpData, size_t timeout);  //参数timeout为生命周期

    void handle_expired_event();

    const static size_t DEFAULT_TIME_OUT = 20 * 1000; // 默认超时时间20s;

private:
    std::priority_queue<Shared_TimerNode, std::deque<Shared_TimerNode>, TimerCmp> TimerQueue;   //截止时间数值小的在队列前面
    mutex_locker lock_;
};

//#endif //CWEBSERVER_TIMER_H
