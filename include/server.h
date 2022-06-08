//
// Created by gsr on 2022/5/14.
//

//#ifndef CWEBSERVER_SERVER_H
//#define CWEBSERVER_SERVER_H

#pragma once

#include "socket.h"
#include "http_data.h"
#include "http_request.h"
#include "http_response.h"

class HttpServer {
public:

    // Http服务器构造函数，默认端口80，监听所有端口
    explicit HttpServer(int port = 80, const char *ip = nullptr) {
        thisServerSocket.InitServer(port);
    }

    void run(int thread_num, int max_queue_size = 10000);

    void do_request(std::shared_ptr<void> arg);

    static void handleConnection(ServerSocket &serverSocket);

private:
    ServerSocket thisServerSocket;
    static TimerManager timerManager;
};


//#endif //CWEBSERVER_SERVER_H
