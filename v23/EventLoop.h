#pragma once
#include "Epoll.h"
#include <functional>

class Channel;
class Epoll;

//事件循环类
class EventLoop{
private:
    Epoll *ep_; //一个事件循环只能有一个Epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;  //epoll_wait()超时的回调函数

public:
    EventLoop(); //构建对象ep_
    ~EventLoop();  //在析构函数中销毁ep_

    void run(); //运行事件循环

    void updatechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
};

