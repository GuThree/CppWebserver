//
// Created by gsr on 2022/5/28.
//

//#ifndef CWEBSERVER_HTTP_PARSE_H
//#define CWEBSERVER_HTTP_PARSE_H

#pragma once

#include "http_request.h"
#include <cstdio>
#include <unistd.h>
#include <cctype>
#include <cstring>
#include <sys/socket.h>

class HttpRequestParser {
public:

    enum HTTP_CODE {    // HTTP解析状态枚举集合
        // NO_REQUEST: 还未初始化好request，
        // FINISH_REQUEST: 已经初始化好request，
        // BAD_REQUEST: 初始化request出错
        NO_REQUEST = -1, FINISH_REQUEST = 0, BAD_REQUEST = 1, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION
    };

    static int get_line(int socket, char *buffer, int size);                       //解析一行

    static HTTP_CODE parse_requestline(int client_fd, HttpRequest &request);       //解析请求行

    static HTTP_CODE parse_headers(int client_fd, HttpRequest &request);           //解析请求头

    static HTTP_CODE parse_body(int client_fd, HttpRequest &request);              //解析请求体

    static HTTP_CODE parse_content(int client_fd, HttpRequest &request);           //解析请求内容
};

//#endif //CWEBSERVER_HTTP_PARSE_H
