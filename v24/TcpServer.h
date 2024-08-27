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

    //用于EchoServer类回调
    std::function<void(Connection*)> newconnectioncb_;
    std::function<void(Connection*)> closeconnectioncb_;
    std::function<void(Connection*)> errorconnectioncb_;
    std::function<void(Connection*,std::string &message)> onmessagecb_;
    std::function<void(Connection*)> sendcompletecb_;
    std::function<void(EventLoop*)> timeoutcb_;

public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start();   //启动事件循环
    //根据业务编写回调函数
    void newconnection(Socket* clientsock); //处理新客户端连接请求，在Acceptor类中回调
    void closeconnection(Connection *conn); //关闭客户端连接，在Connection类中回调
    void errorconnection(Connection *conn); //客户端连接错误
    void onmessage(Connection *conn,std::string &message); //处理客户端的请求报文，在Connection类中回调此函数
    void sendcomplete(Connection *conn);    //数据发送完成后，Connection类回调
    void epolltimeout(EventLoop* evl);  //epoll_wait超时，EventLoop类中回调

    void setnewconnectioncb(std::function<void(Connection*)> fn);
    void setcloseconnectioncb(std::function<void(Connection*)> fn);
    void seterrorconnectioncb(std::function<void(Connection*)> fn);
    void setonmessagecb(std::function<void(Connection*, std::string &message)> fn);
    void setsendcompletecb(std::function<void(Connection*)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);

};