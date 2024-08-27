#pragma once

#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class EventLoop;
class Channel;

class Connection{
private:
    EventLoop *evl_;    //对应事件循环，由构造函数传入
    Socket *clientsock_;    //与客户端通讯的Socket
    Channel *clientchannel_;    //Connection对应channel
    Buffer inputbuffer_;     //接收缓冲区
    Buffer outputbuffer_;    //发送缓冲区

    std::function<void(Connection *)> closecallback_; //回调TcpServer::closeconnection()
    std::function<void(Connection *)> errorcallback_; //回调TcpServer::errorconnection()

public:
    Connection(EventLoop *evl, Socket *clientsock);
    ~Connection();

    int getfd() const;
    std::string getip() const;
    uint16_t getport() const;

    void onmessage();   //处理对端发送过来的消息
    void closecallback();   //TCP连接关闭回调函数
    void errorcallback();   //TCP连接错误的回调函数

    void setclosecallback(std::function<void(Connection *)> fn);//设置回调函数
    void seterrorcallback(std::function<void(Connection *)> fn);
};