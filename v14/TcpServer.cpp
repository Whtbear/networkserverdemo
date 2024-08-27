#include "TcpServer.h"

/*
class TcpServer{
private:
    EventLoop evl_; //目前只用单线程，一个事件循环
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();


};
*/

TcpServer::TcpServer(const std::string &ip, const uint16_t port){
    acceptor_ = new Acceptor(&evl_,ip,port);

}

TcpServer::~TcpServer(){
    delete acceptor_;
}

void TcpServer::start(){
    evl_.run();
}