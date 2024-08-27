#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

/*
    这里用于进行回显业务逻辑的编写
*/

class EchoServer{
private:
    TcpServer ts_;
    ThreadPool threadpool_; //工作线程池

public:
    EchoServer(const std::string &ip, const uint16_t port, int subthreadnum = 5, int workthreadnum = 5);
    ~EchoServer();

    void start();    //启动服务

    void handlenewconnection(spConnection conn); //处理新客户端连接请求，TcpServer类中回调
    void handleclose(spConnection conn); //关闭客户端连接，TcpServer类中回调
    void handleerror(spConnection conn); //客户端连接错误，TcpServer类中回调
    void handlemessage(spConnection conn,std::string &message); //处理客户端的请求报文，TcpServer类中回调
    void handlesendcomplete(spConnection conn);    //数据发送完成后，TcpServer类中回调
    void handletimeout(EventLoop* evl);  //epoll_wait超时，TcpServer类中回调

    void onmessage(spConnection conn, std::string &message); //处理客户端的请求报文，用于添加给线程池
};