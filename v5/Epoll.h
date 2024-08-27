#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>

//Epoll类
class Epoll{
private:
    int epollfd_ = -1;  //epoll句柄
    static const int Max_Events = 100;
    epoll_event events_[Max_Events];
public:
    Epoll();    //构造函数中创建epollfd句柄
    ~Epoll();   //析构函数中关闭epollfd

    void addfd(int fd,uint32_t op);//把fd和需要监视的事件添加到红黑树
    std::vector<epoll_event> loopwait(int timeout=-1);//运行epoll_wait()，等待事件发生
    //发生事件用vector返回

};