#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>  //map用于管理connection对象

class TcpServer{
private:
    EventLoop evl_; //目前只用单线程，一个事件循环
    Acceptor *acceptor_;    //与TcpServer 1:1对应
    std::map<int,Connection*> conns_;   //fd -> conn将多个Connection对象放入map容器进行管理

public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start();   //启动事件循环

    void newconnection(Socket* clientsock);
};