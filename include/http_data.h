//
// Created by gsr on 2022/5/29.
//

//#ifndef CWEBSERVER_HTTP_DATA_H
//#define CWEBSERVER_HTTP_DATA_H

#pragma once

#include "http_request.h"
#include "http_response.h"
#include "socket.h"
#include "timer.h"
#include <memory>

class TimerNode;

class HttpData{
public:
    HttpData() : epoll_fd(-1) {}

public:
    std::shared_ptr<HttpRequest> request_;
    std::shared_ptr<HttpResponse> response_;
    std::shared_ptr<ClientSocket> clientSocket_;
    int epoll_fd;

    void closeTimer();
    void setTimer(std::shared_ptr<TimerNode>);

private:
    std::weak_ptr<TimerNode> timer;
};
//#endif //CWEBSERVER_HTTP_DATA_H
