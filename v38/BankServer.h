#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

/*
    这里用于进行回显业务逻辑的编写
*/

class UserInfo{     //用户信息
private:
    int fd_;
    std::string ip_;
    bool login_ = false;    //客户端登录状态
    std::string name_;  //客户端用户名
public:
    UserInfo(int fd,const std::string &ip):fd_(fd),ip_(ip){}
    void setLogin(bool login) {login_ = login;}
    bool isLogin() {return login_;}
};

class BankServer{
private:
    using spUserInfo = std::shared_ptr<UserInfo>;
    TcpServer ts_;
    ThreadPool threadpool_; //工作线程池
    std::mutex mutex_;     //usermap_的互斥锁
    std::map<int,spUserInfo> usermap_;  //用户状态机

public:
    BankServer(const std::string &ip, const uint16_t port, int subthreadnum = 5, int workthreadnum = 5);
    ~BankServer();

    void start();    //启动服务
    void stop();

    void handlenewconnection(spConnection conn); //处理新客户端连接请求，TcpServer类中回调
    void handleclose(spConnection conn); //关闭客户端连接，TcpServer类中回调
    void handleerror(spConnection conn); //客户端连接错误，TcpServer类中回调
    void handlemessage(spConnection conn,std::string &message); //处理客户端的请求报文，TcpServer类中回调
    void handlesendcomplete(spConnection conn);    //数据发送完成后，TcpServer类中回调
    void handletimeout(EventLoop* evl);  //epoll_wait超时，TcpServer类中回调
    void handleremove(int fd);  //客户端连接超时，TcpServer回调函数

    void onmessage(spConnection conn, std::string &message); //处理客户端的请求报文，用于添加给线程池
};