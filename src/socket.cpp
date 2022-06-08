//
// Created by gsr on 2022/4/21.
//

#include "../include/socket.h"

ServerSocket::ServerSocket()
{
    m_listenfd = -1;
    m_connfd = -1;
    m_socklen = 0;
}

bool ServerSocket::InitServer(const unsigned int port)
{
    if (m_listenfd > 0) {
        close(m_listenfd);
        m_listenfd = -1;
    }

    m_listenfd = socket(AF_INET,SOCK_STREAM,0);

    int opt = 1;
    unsigned int len = sizeof(opt);
    setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,len);

    memset(&m_servaddr,0,sizeof(m_servaddr));
    m_servaddr.sin_family = AF_INET;
    m_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_servaddr.sin_port = htons(port);
    if (bind(m_listenfd,(struct sockaddr *)&m_servaddr,sizeof(m_servaddr)) != 0 )
    {
        CloseListen();
        exit(0);
    }

    if (listen(m_listenfd,5) != 0 )
    {
        CloseListen();
        exit(0);
    }

    m_socklen = sizeof(struct sockaddr_in);

    return true;
}

bool ServerSocket::Accept()
{
    if (m_listenfd == -1) return false;

    if ((m_connfd=accept(m_listenfd,(struct sockaddr *)&m_clientaddr,(socklen_t*)&m_socklen)) < 0)
        perror("accept error");

    return true;
}

char *ServerSocket::GetIP()
{
    return(inet_ntoa(m_clientaddr.sin_addr));
}

void ServerSocket::CloseListen()
{
    if (m_listenfd > 0) {
        close(m_listenfd);
        m_listenfd=-1;
    }
}

void ServerSocket::CloseClient()
{
    if (m_connfd > 0) {
        close(m_connfd);
        m_connfd = -1;
    }
}

ServerSocket::~ServerSocket()
{
    CloseListen();
    CloseClient();
}

//============================================//

ClientSocket::ClientSocket()
{
    m_sockfd = -1;
    memset(m_ip,0,sizeof(m_ip));
    m_port = 0;
}

void ClientSocket::Close()
{
    if (m_sockfd > 0)
        close(m_sockfd);

    m_sockfd = -1;
    memset(m_ip,0,sizeof(m_ip));
    m_port = 0;
}

ClientSocket::~ClientSocket()
{
    Close();
}