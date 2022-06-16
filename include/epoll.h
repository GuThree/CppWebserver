//
// Created by gsr on 2022/5/27.
//

//#ifndef CWEBSERVER_EPOLL_H
//#define CWEBSERVER_EPOLL_H

#pragma once

#include <sys/epoll.h>
#include <iostream>
#include <vector>
#include <sys/fcntl.h>
#include <unordered_map>
#include <memory>

#include "socket.h"
#include "http_data.h"
#include "timer.h"
#include "server.h"


class Epoll {

public:
    static int init(int max_events);

    static int addFd(int epoll_fd, int fd, __uint32_t events, std::shared_ptr<HttpData>);

    static int modFd(int epoll_fd, int fd, __uint32_t events, std::shared_ptr<HttpData>);

    static int delFd(int epoll_fd, int fd);

    static std::vector<std::shared_ptr<HttpData> > poll(const ServerSocket &serverSocket, int max_event, int timeout);  //轮询

public:
    static std::unordered_map<int, std::shared_ptr<HttpData> > httpDataMap;         //一个fd对应一个httpdata
    static epoll_event *events;
    const static __uint32_t DEFAULT_EVENTS = (EPOLLIN | EPOLLET | EPOLLONESHOT);    //默认事件
};

//#endif //CWEBSERVER_EPOLL_H
