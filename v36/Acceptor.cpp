#include "Acceptor.h"
#include "Connection.h"

Acceptor::Acceptor(EventLoop *evl, const std::string &ip, const uint16_t port):evl_(evl),servsock_(create_nonblocking()),acceptchannel_(evl_,servsock_.getfd()){
    //servsock_ = new Socket(create_nonblocking());
    InetAddress servaddr(ip, port);//创建结构地址
    servsock_.setreuseaddr(true);
    servsock_.settcpnodelay(true);
    servsock_.setreuseport(true);
    servsock_.setkeepalive(true);
    servsock_.bind(servaddr);
    servsock_.listen();  //默认参数

    //acceptchannel_ = new Channel(evl_, servsock_.getfd());
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnection, this));
    acceptchannel_.enablereading();   //监视读事件
}

Acceptor::~Acceptor(){
    //delete servsock_;
    //delete acceptchannel_;
}

void Acceptor::newconnection(){
    InetAddress clientaddr;  //封装了客户端地址和协议
    
    std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
    //此处的sock通过智能指针自动管理并释放
    clientsock->setipnport(clientaddr.getip(),clientaddr.getport());

    //Connection *conn = new Connection(evl_,clientsock); //此处对象暂未释放处理
    newconnectionts_(std::move(clientsock));

}

void Acceptor::setnewconnectionts(std::function<void(std::unique_ptr<Socket>)> fn){
    newconnectionts_ = fn;


}
