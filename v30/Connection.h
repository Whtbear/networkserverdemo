#pragma once

#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>

class Connection;
using spConnection = std::shared_ptr<Connection>;   //定义智能指针

class EventLoop;
class Channel;

class Connection:public std::enable_shared_from_this<Connection>{
private:
    const std::unique_ptr<EventLoop>& evl_;    //对应事件循环，由构造函数传入
    std::unique_ptr<Socket> clientsock_;    //与客户端通讯的Socket
    std::unique_ptr<Channel> clientchannel_;    //Connection对应channel
    Buffer inputbuffer_;     //接收缓冲区
    Buffer outputbuffer_;    //发送缓冲区
    std::atomic_bool isdisconnect_;   //判断客户端连接是否已断开，如果已断开，则设置为true


    std::function<void(spConnection)> closecallback_; //回调TcpServer::closeconnection()
    std::function<void(spConnection)> errorcallback_; //回调TcpServer::errorconnection()
    std::function<void(spConnection, std::string&)> onmessagecallback_;  //回调 TcpServer::onmessage()。
    std::function<void(spConnection)> sendcompletecallback_;//通知完成的回调函数，TcpServer::sendcomplete()

public:
    Connection(const std::unique_ptr<EventLoop>& evl, std::unique_ptr<Socket> clientsock);
    ~Connection();

    int getfd() const;
    std::string getip() const;
    uint16_t getport() const;

    void onmessage();   //处理对端发送过来的消息
    void closecallback();   //TCP连接关闭回调函数
    void errorcallback();   //TCP连接错误的回调函数
    void writecallback();   //处理写事件的回调函数，供Channel回调

    void setclosecallback(std::function<void(spConnection)> fn);//设置回调函数
    void seterrorcallback(std::function<void(spConnection)> fn);
    void setonmessagecallback(std::function<void(spConnection, std::string&)> fn);
    void setsendcompletecallback(std::function<void(spConnection)> fn);  //设置数据发送完成回调

    void send(const char *data,size_t size);

};