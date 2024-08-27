#pragma once

#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class EventLoop;
class Channel;

class Connection{
private:
    EventLoop *evl_;    //对应事件循环，由构造函数传入
    Socket *clientsock_;    //与客户端通讯的Socket
    Channel *clientchannel_;    //Connection对应channel

public:
    Connection(EventLoop *evl, Socket *clientsock);
    ~Connection();

    int getfd() const;
    std::string getip() const;
    uint16_t getport() const;
};