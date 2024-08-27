#pragma once
#include "Epoll.h"
#include "Connection.h"
#include <functional>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <map>
#include <atomic>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <memory>

class Channel;
class Epoll;
class Connection;
using spConnection = std::shared_ptr<Connection>;


//事件循环类
class EventLoop{
private:
    std::unique_ptr<Epoll> ep_; //一个事件循环只能有一个Epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;  //epoll_wait()超时的回调函数
    int timetvl_;   //闹钟的时间间隔
    int timeout_;   //Connection对象超时的时间
    pid_t threadid_;    //事件循环线程id
    std::queue<std::function<void()>> taskqueue_;
    std::mutex mutex_;
    int wakeefd_;       //唤醒事件循环线程eventfd
    std::unique_ptr<Channel> wakechannel_;   //eventfd对应的channel
    int timerfd_;       //定时器的fd
    std::unique_ptr<Channel> timerchannel_; //定时器的Channel
    bool ismainevl_;    //判断是否为主事件循环 true为是，false为否
    std::mutex evlmutex_;   
    std::map<int,spConnection> conns_;  //<fd,con的智能指针>，存放该事件循环上全部Connection对象
    std::function<void(int)> timercallback_;    //在TcpSever中被设置，删除TcpServer超时的Connection对象
    std::atomic_bool stop_; //事件循环标志

public:
    EventLoop(bool ismainevl, int timetvl=30, int timeout = 80); //构建对象ep_
    ~EventLoop();  //在析构函数中销毁ep_

    void run(); //运行事件循环
    void stop();  // 停止事件循环

    void updatechannel(Channel *ch);    //channel被更新到红黑树上
    void removechannel(Channel *ch);    //从红黑树删除channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);

    bool isloopthread();    //判断当前线程是否为事件循环线程

    void queueinloop(std::function<void()> fn);     //把任务添加到队列中
    void wakeup();  //用eventfd唤醒事件循环
    void handlewakeup();    //唤醒后执行函数

    void handletimer();     //定时器响时处理函数

    void newconnection(spConnection conn);  //将Connection对象保存在conns_中
    void settimercallback(std::function<void(int)> fn);    //将被设置为TcpServer::removeconn()

};

