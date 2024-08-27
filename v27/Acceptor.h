#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

//这个类主要用于处理servsock和channel的释放
class Acceptor{
private:
    EventLoop *evl_; //接收acceptor 与事件循环1对1，构造函数传入
    Socket *servsock_;//对应于服务端监听socket
    Channel *acceptchannel_;//对应channel
    std::function<void(Socket*)> newconnectionts_;//处理新客户端连接请求回调函数，由TcpServer调用

public:
    Acceptor(EventLoop *evl, const std::string &ip, const uint16_t port);
    ~Acceptor();

    void newconnection();

    void setnewconnectionts(std::function<void(Socket*)> fn);   //设置新客户端连接请求的回调函数
};