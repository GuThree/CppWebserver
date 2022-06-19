//
// Created by ASUS on 2022/6/1.
//

#include <sys/fcntl.h>
#include "../include/utility.h"
#include "../include/logger.h"

//当系统中断时，关闭日志文件
void signal_handler(int sig){
    LogFile::writeInfo("The server is down\n");
    LogFile::closeFile();
}

int setNonblocking(int fd) {                    //将文件描述符设置为不可阻塞的状态
    int old_option = fcntl(fd, F_GETFL);        //获得原有的描述符状态标记
    int new_option = old_option | O_NONBLOCK;   //设置非阻塞操作位，这样子客户端接上套接字时才能进行写操作。
    fcntl(fd, F_SETFL, new_option);             //把新的状态标记设置回去
    return old_option;                          //返回原有的状态标记
}