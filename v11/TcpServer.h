#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

class TcpServer{
private:
    EventLoop evl_; //目前只用单线程，一个事件循环
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start();   //启动事件循环
};