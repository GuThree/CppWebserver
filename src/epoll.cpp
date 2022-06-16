//
// Created by gsr on 2022/5/27.
//


#include "../include/epoll.h"

epoll_event *Epoll::events;
std::unordered_map<int, std::shared_ptr<HttpData> > Epoll::httpDataMap;

int Epoll::init(int max_events) {
    int epoll_fd = ::epoll_create(max_events);
    if (epoll_fd == -1) {
        std::cout << "epoll create error" << std::endl;
        exit(-1);
    }
    events = new epoll_event[max_events];   // 为epoll创建用户态下用于接受内核态的epoll_event事件的缓存
    return epoll_fd;
}

int Epoll::addFd(int epoll_fd, int fd, __uint32_t events, std::shared_ptr<HttpData> httpData) {
    epoll_event event;              // 初始化epoll_event
    event.events = events;          // 设置该event的监听事件列表
    event.data.fd = fd;             // 并且把携带的数据设置为用户描述符
    httpDataMap[fd] = httpData;     // 在httpDataMap中设置该文件描述符的数据
    int ret = ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);     // 往epoll中添加该文件描述符对应的事件

    if (ret < 0) {  //
        std::cout << "epoll add error" << std::endl;
        httpDataMap[fd].reset();
        return -1;
    }

    return 0;
}

int Epoll::modFd(int epoll_fd, int fd, __uint32_t events, std::shared_ptr<HttpData> httpData) {
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    httpDataMap[fd] = httpData;
    int ret = ::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);     // 和addFd相比只有这一行的参数2改变了

    if (ret < 0) {
        std::cout << "epoll mod error" << std::endl;
        httpDataMap[fd].reset();
        return -1;
    }

    return 0;
}


int Epoll::delFd(int epoll_fd, int fd) {

    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);   // 删除文件描述符指定的事件监听
    if (ret < 0) {
        std::cout << "epoll del error" << std::endl;
        return -1;
    }
    // epoll里删完了还不算，要在httpDataMap里面一块删除了才可以
    auto it = httpDataMap.find(fd);
    if (it != httpDataMap.end()) {
        httpDataMap.erase(it);
    }
    return 0;
}

// 轮询
std::vector<std::shared_ptr<HttpData>> Epoll::poll(const ServerSocket &serverSocket, int max_event, int timeout) {
    std::cout << "epoll_waiting......\n";
    int event_num = epoll_wait(serverSocket.epoll_fd, events, max_event, timeout);      // 接受需要处理的事件，返回值为需要处理的事件数目
    if (event_num < 0) {
        std::cout << "epoll_num=" << event_num << std::endl;
        std::cout << "epoll_wait error" << std::endl;
        std::cout << errno << std::endl;
        exit(-1);
    }

    std::vector<std::shared_ptr<HttpData>> httpDatas;

    for (int i = 0; i < event_num; i++) {
        int fd = events[i].data.fd;

        if (fd == serverSocket.m_listenfd) {    // 如果是监听端口有事件要处理，那必然是有新的链接请求，当场处理
            HttpServer::handleConnection(const_cast<ServerSocket &>(serverSocket));
        } else {

            // 如果出错，或者客户端或自己挂起了链接
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP)) {
                auto it = httpDataMap.find(fd);
                if (it != httpDataMap.end()) {
                    it->second->closeTimer();   // 抬出去，清空定时器，清除位于HttpDataMap中的记录，定时器析构释放HttpData指针对应的对象
                }
                continue;
            }

            //正常事件
            auto it = httpDataMap.find(fd);
            if (it != httpDataMap.end()) {
                if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI)) {    // 只管输入和紧急事件可读两种
                    httpDatas.push_back(it->second);    //准备要干活的httpData
                    it->second->closeTimer();
                    httpDataMap.erase(it);
                }
            } else {
                std::cout << "长连接第二次连接未找到" << std::endl;
                close(fd);
                continue;
            }
        }
    }

    return httpDatas;           //返回要干活的httpdata数组
}

