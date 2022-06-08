//
// Created by gsr on 2022/5/29.
//

#include "../include/http_data.h"

void HttpData::closeTimer() {
    if (this->timer.lock()) {               //判断其指向的shared_ptr内存对象是否依然有效
        std::shared_ptr<TimerNode> tempTimer(this->timer.lock());
        tempTimer->deleted();
        this->timer.reset();
    }
}

void HttpData::setTimer(std::shared_ptr<TimerNode> timer) {
    this->timer = timer;                     //一个weak_ptr指向shared_ptr
}