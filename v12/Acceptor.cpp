#include "Acceptor.h"

/*
class Acceptor{
private:
    EventLoop *evl_; //接收acceptor 与事件循环1对1，构造函数传入
    Socket *servsock_;//对应于服务端监听socket
    Channel *acceptchannel_;//对应channel
public:
    Acceptor(EventLoop *evl, const std::string &ip, const uint16_t port);
    ~Acceptor();
};
*/

Acceptor::Acceptor(EventLoop *evl, const std::string &ip, const uint16_t port):evl_(evl){
    servsock_ = new Socket(create_nonblocking());
    InetAddress servaddr(ip, port);//创建结构地址
    servsock_ -> setreuseaddr(true);
    servsock_ -> settcpnodelay(true);
    servsock_ -> setreuseport(true);
    servsock_ -> setkeepalive(true);
    servsock_ -> bind(servaddr);
    servsock_ -> listen();  //默认参数

    acceptchannel_ = new Channel(evl_, servsock_ -> getfd());
    acceptchannel_ -> setreadcallback(std::bind(&Channel::newconnection, acceptchannel_, servsock_));
    acceptchannel_ -> enablereading();   //监视读事件
}

Acceptor::~Acceptor(){
    delete servsock_;
    delete acceptchannel_;
}
