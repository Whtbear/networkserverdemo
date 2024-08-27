#include "Connection.h"

/*
class Connection{
private:
    EventLoop *evl_;    //对应事件循环，由构造函数传入
    Socket *clientsock_;    //与客户端通讯的Socket
    Channel *clientchannel_;    //Connection对应channel

public:
    Connection(EventLoop *evl, Socket *clientsock);
    ~Connection();
};
*/

Connection::Connection(EventLoop *evl, Socket *clientsock):evl_(evl),clientsock_(clientsock){
    clientchannel_ = new Channel(evl_, clientsock_ -> getfd());
    clientchannel_ -> setreadcallback(std::bind(&Channel::onmessage,clientchannel_));
    clientchannel_ -> setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_ -> seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_ -> enablereading();
    clientchannel_ -> setet();//改为边缘触发模式

}

Connection::~Connection(){
    delete clientsock_;
    delete clientchannel_;

}

int Connection::getfd() const{
    return clientsock_ -> getfd();
}

std::string Connection::getip() const{
    return clientsock_ -> getip();
}

uint16_t Connection::getport() const{
    return clientsock_ -> getport();
}

void Connection::closecallback(){
    printf("客户端(eventfd=%d) 关闭连接。\n", clientsock_ -> getfd());
    close(clientsock_ -> getfd());
}   

void Connection::errorcallback(){
    printf("客户端(eventfd=%d) 错误。\n",clientsock_ -> getfd());
    close(clientsock_ -> getfd());

}   
