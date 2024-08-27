#pragma once
#include "Epoll.h"
#include <functional>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <memory>

class Channel;
class Epoll;

//事件循环类
class EventLoop{
private:
    std::unique_ptr<Epoll> ep_; //一个事件循环只能有一个Epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;  //epoll_wait()超时的回调函数
    pid_t threadid_;    //事件循环线程id
    std::queue<std::function<void()>> taskqueue_;
    std::mutex mutex_;
    int wakeefd_;       //唤醒事件循环线程eventfd
    std::unique_ptr<Channel> wakechannel_;   //eventfd对应的channel
    int timerfd_;       //定时器的fd
    std::unique_ptr<Channel> timerchannel_; //定时器的Channel
    bool ismainevl_;    //判断是否为主事件循环 true为是，false为否


public:
    EventLoop(bool ismainevl); //构建对象ep_
    ~EventLoop();  //在析构函数中销毁ep_

    void run(); //运行事件循环

    void updatechannel(Channel *ch);    //channel被更新到红黑树上
    void removechannel(Channel *ch);    //从红黑树删除channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);

    bool isloopthread();    //判断当前线程是否为事件循环线程

    void queueinloop(std::function<void()> fn);     //把任务添加到队列中
    void wakeup();  //用eventfd唤醒事件循环
    void handlewakeup();    //唤醒后执行函数

    void handletimer();     //定时器响时处理函数
};

