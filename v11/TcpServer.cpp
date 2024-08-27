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
    Socket *servsock = new Socket(create_nonblocking());
    InetAddress servaddr(ip, port);//创建结构地址
    servsock -> setreuseaddr(true);
    servsock -> settcpnodelay(true);
    servsock -> setreuseport(true);
    servsock -> setkeepalive(true);
    servsock -> bind(servaddr);
    servsock -> listen();  //默认参数

    Channel *servchannel = new Channel(&evl_, servsock -> getfd());
    servchannel -> setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    servchannel -> enablereading();   //监视读事件


}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    evl_.run();
}