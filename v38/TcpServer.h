#pragma once

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>  //map用于管理connection对象
#include <memory>
#include <mutex>

class TcpServer{
private:
    std::unique_ptr<EventLoop> mainevl_; //主线程事件循环
    std::vector<std::unique_ptr<EventLoop>> subevls_;    //从事件循环容器用vector保存
    Acceptor acceptor_;    //与TcpServer 1:1对应
    std::map<int,spConnection> conns_;   //fd -> conn将多个Connection对象放入map容器进行管理
    int threadnum_;     //线程池大小，也可以理解为同时管理的从事件个数
    ThreadPool threadpool_;    //线程池指针
    std::mutex evlmutex_;   //多个事件线程访问和修改conns需要加锁

    //用于EchoServer类回调
    std::function<void(spConnection)> newconnectioncb_;
    std::function<void(spConnection)> closeconnectioncb_;
    std::function<void(spConnection)> errorconnectioncb_;
    std::function<void(spConnection,std::string &message)> onmessagecb_;
    std::function<void(spConnection)> sendcompletecb_;
    std::function<void(EventLoop*)> timeoutcb_;
    std::function<void(int)> removeconnectioncb_;

public:
    TcpServer(const std::string &ip, const uint16_t port, int threadnum = 5);
    ~TcpServer();

    void start();   //启动事件循环
    void stop();    //停止IO线程和事件循环

    //根据业务编写回调函数
    void newconnection(std::unique_ptr<Socket> clientsock); //处理新客户端连接请求，在Acceptor类中回调
    void closeconnection(spConnection conn); //关闭客户端连接，在Connection类中回调
    void errorconnection(spConnection conn); //客户端连接错误
    void onmessage(spConnection conn,std::string &message); //处理客户端的请求报文，在Connection类中回调此函数
    void sendcomplete(spConnection conn);    //数据发送完成后，Connection类回调
    void epolltimeout(EventLoop* evl);  //epoll_wait超时，EventLoop类中回调

    void setnewconnectioncb(std::function<void(spConnection)> fn);
    void setcloseconnectioncb(std::function<void(spConnection)> fn);
    void seterrorconnectioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection, std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
    void setremoveconnectioncb(std::function<void(int)> fn);

    void removeconn(int fd);    //删除conns_中的Connection对象，在EventLoop::handletimer()中回调
};