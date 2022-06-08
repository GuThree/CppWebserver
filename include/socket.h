//
// Created by gsr on 2022/4/21.
//

//#ifndef CWEBSERVER_SOCKET_H
//#define CWEBSERVER_SOCKET_H

#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

//socket通信的服务端类
class ServerSocket
{
private:
    int m_socklen;                    // 结构体struct sockaddr_in的大小。
    struct sockaddr_in m_clientaddr;  // 客户端的地址信息。
    struct sockaddr_in m_servaddr;    // 服务端的地址信息。
public:
    int  m_listenfd;   // 服务端用于监听的socket。
    int  m_connfd;     // 客户端连接上来的socket。
    int epoll_fd;

    ServerSocket();

    bool InitServer(const unsigned int port);

    bool Accept();

    char *GetIP();  // 获取客户端的ip地址。返回值：客户端的ip地址，如"192.168.1.100"

    void CloseListen();

    void CloseClient();

    ~ServerSocket();
};
//socket通信的客户端类
class ClientSocket
{
public:
    int  m_sockfd;    // 客户端的socket.
    char m_ip[21];    // 服务端的ip地址。
    int  m_port;      // 与服务端通信的端口。

    void Close();
    ClientSocket();
    ~ClientSocket();
};

//#endif //CWEBSERVER_SOCKET_H
