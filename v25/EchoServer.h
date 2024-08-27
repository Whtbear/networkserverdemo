#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"

/*
    这里用于进行回显业务逻辑的编写
*/

class EchoServer{
private:
    TcpServer ts_;

public:
    EchoServer(const std::string &ip, const uint16_t port);
    ~EchoServer();

    void start();    //启动服务

    void handlenewconnection(Connection *conn); //处理新客户端连接请求，TcpServer类中回调
    void handleclose(Connection *conn); //关闭客户端连接，TcpServer类中回调
    void handleerror(Connection *conn); //客户端连接错误，TcpServer类中回调
    void handlemessage(Connection *conn,std::string &message); //处理客户端的请求报文，TcpServer类中回调
    void handlesendcomplete(Connection *conn);    //数据发送完成后，TcpServer类中回调
    void handletimeout(EventLoop* evl);  //epoll_wait超时，TcpServer类中回调
};