#pragma once

#include <functional>
#include <sys/epoll.h>
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"
#include <memory>

class EventLoop;//显式声明Epoll

class Channel{
private:
    int fd_ = -1; //Channel  与   fd 1:1
    //Epoll *ep_ = nullptr;   //红黑树   Channel与Epoll  n:1  而一个Channel只对应一个Epoll
    EventLoop *evl_;
    bool inepoll_ = false;  //Channel是否已被添加到epoll的红黑树上，false则在调用epoll_ctl()时用EPOLL_CTL_ADD
    //否则用EPOLL_CTL_MOD
    uint32_t events_ = 0;   //fd_需要监视的事件，listenfd和clientfd需要监视EPOLLIN，而clientfd还可能需要监视EPOLLOUT
    uint32_t revents_ = 0;  //fd_已发生的事件
    std::function <void()> readcallback_;   //fd_读事件回调函数
    std::function <void()> closecallback_;  //关闭fd_的回调函数，将回调Connection::closecallback()。
    std::function <void()> errorcallback_;  //fd_发生了错误的回调函数，将回调Connection::errorcallback()
    std::function <void()> writecallback_;  //fd_写事件的回调函数，将回调Connection::writecallback()。

public:
    Channel(EventLoop *evl,int fd); //构造函数
    ~Channel();     //析构函数

    int getfd();        //返回fd_成员
    void setet();        //切换成边缘触发模式
    void enablereading();   //让epoll_wait()监视fd_的读事件
    void disablereading();  //取消读事件
    void enablewriting();   //注册写事件
    void disablewriting();  //取消写事件
    void disableall();  //取消全部事件
    void remove();      //从事件循环删除Channel
    void setinepoll();  //设置inepoll的成员值为true
    void setrevents(uint32_t ev);   //设置revents_的成员的值为ev
    bool getinepoll();  
    uint32_t getevents();
    uint32_t getrevents();

    void handleevent(); //事件处理，在epoll_wait后执行
    
    void setreadcallback(std::function<void()> fn);  //设置读事件回调函数
    void setclosecallback(std::function<void()> fn);   //设置fd_的回调函数，将回调Connection::closecallback()。
    void seterrorcallback(std::function<void()> fn);   //设置fd_的回调函数，将回调Connection::errorcallback()。
    void setwritecallback(std::function<void()> fn);   //设置写事件的回调函数
};