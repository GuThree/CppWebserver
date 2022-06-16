//
// Created by gsr on 2022/5/14.
//

#include "../include/server.h"
#include "../include/epoll.h"
#include "../include/http_parse.h"
#include "../include/threadpool.h"


TimerManager HttpServer::timerManager;

int setNonblocking(int fd) {                    //将文件描述符设置为不可阻塞的状态
    int old_option = fcntl(fd, F_GETFL);        //获得原有的描述符状态标记
    int new_option = old_option | O_NONBLOCK;   //设置非阻塞操作位，这样子客户端接上套接字时才能进行写操作。
    fcntl(fd, F_SETFL, new_option);             //把新的状态标记设置回去
    return old_option;                          //返回原有的状态标记
}

void HttpServer::run(int thread_num, int max_queue_size) {
    threadpool threadPool(thread_num, max_queue_size);                       //创建线程池
    int epoll_fd = Epoll::init(1024);                              //初始化epoll，epoll_fd全局唯一

    std::shared_ptr<HttpData> httpData(new HttpData());                   //listen_fd附带的httpdata，因为不是client_fd，所以实际没什么用途，走个形式
    httpData->epoll_fd = epoll_fd;
    thisServerSocket.epoll_fd = epoll_fd;

    __uint32_t event = (EPOLLIN | EPOLLET);                                  // 边缘触发模式下监听输入事件
    Epoll::addFd(epoll_fd, thisServerSocket.m_listenfd, event, httpData);    // 添加服务器监听端口的首要监听事件

    while (true) {          // 从EPOLL中轮询获取工作
        std::vector<std::shared_ptr<HttpData>> todo = Epoll::poll(thisServerSocket, 1024, -1);        //返回要干活的httpdata数组
        //把要做的事情交给线程池
        for (auto& req : todo) {
            threadPool.addjob(req, std::bind(&HttpServer::do_request, this, std::placeholders::_1));
        }
        // 处理定时器超时事件
        this->timerManager.handle_expired_event();
    }
}

void HttpServer::do_request(std::shared_ptr<void> arg) {
    std::shared_ptr<HttpData> httpdata_worker = std::static_pointer_cast<HttpData>(arg);

    HttpRequestParser::HTTP_CODE retcode = HttpRequestParser::parse_content(httpdata_worker->clientSocket_->m_sockfd, *httpdata_worker->request_);

    std::cout << httpdata_worker->clientSocket_->m_sockfd << "clientFD: HTTP_CODE: " << retcode << std::endl;

    if (retcode == HttpRequestParser::NO_REQUEST) {
            return;
    } else if (retcode == HttpRequestParser::FINISH_REQUEST) {
        //检查keep_alive选项
        auto it = httpdata_worker->request_->mHeaders.find(HttpRequest::Connection);
        if (it != httpdata_worker->request_->mHeaders.end()) {
            if (it->second == "keep-alive") {
                httpdata_worker->response_->setKeepAlive(true);
                // timeout=20s
                httpdata_worker->response_->addHeader("Keep-Alive", std::string("timeout=20"));
            } else {
                httpdata_worker->response_->setKeepAlive(false);
            }
        }

        httpdata_worker->response_->analysis(retcode, *httpdata_worker->request_);

        //给客户机发送response
        send(httpdata_worker->clientSocket_->m_sockfd, httpdata_worker->response_->response_message, strlen(httpdata_worker->response_->response_message), 0);

        // 如果是keep_alive else sharedHttpData将会自动析构释放clientSocket，从而关闭资源
        if (httpdata_worker->response_->isKeepAlive()) {
            Epoll::modFd(httpdata_worker->epoll_fd, httpdata_worker->clientSocket_->m_sockfd, Epoll::DEFAULT_EVENTS, httpdata_worker);
            this->timerManager.addTimer(httpdata_worker, TimerManager::DEFAULT_TIME_OUT);
        }
    } else {
        httpdata_worker->response_->analysis(retcode, *httpdata_worker->request_);
        ::send(httpdata_worker->clientSocket_->m_sockfd, httpdata_worker->response_->response_message, strlen(httpdata_worker->response_->response_message), 0);
    }
}


void HttpServer::handleConnection(ServerSocket &serverSocket){
    std::shared_ptr<ClientSocket> tempClient(new ClientSocket); // 创建一个指定特定客户端的套接字

    if (serverSocket.Accept()) {  // 当我们正式创建一个链接
        std::cout << serverSocket.m_connfd << " clientfd accepted.\n";

        tempClient->m_sockfd = serverSocket.m_connfd;
        int ret = setNonblocking(tempClient->m_sockfd);
        if (ret < 0) {
            std::cout << "setNonblocking error" << std::endl;
            tempClient->Close();
            return;
        }

        std::shared_ptr<HttpData> sharedHttpData(new HttpData); // 初始化一个HttpData，并且在下面设置它的属性
        sharedHttpData->request_ = std::shared_ptr<HttpRequest>(new HttpRequest);
        sharedHttpData->response_ = std::shared_ptr<HttpResponse>(new HttpResponse);

        std::shared_ptr<ClientSocket> sharedClientSocket(new ClientSocket);
        sharedClientSocket.swap(tempClient);    // 交换两个共享指针的值
        sharedHttpData->clientSocket_ = sharedClientSocket;
        sharedHttpData->epoll_fd = serverSocket.epoll_fd;

        // 将这个Http链接加入到epoll事件监听中，监听默认事件。
        // 并且把对应的httpData指针放入httpDataMap中
        Epoll::addFd(serverSocket.epoll_fd, sharedClientSocket->m_sockfd, Epoll::DEFAULT_EVENTS, sharedHttpData);
        // 为这个Http链接添加计时器
        timerManager.addTimer(sharedHttpData, TimerManager::DEFAULT_TIME_OUT);
    } else {
        std::cout << "Client connection error\n";
    }

}