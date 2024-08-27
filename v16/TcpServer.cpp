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
    //设置回调函数
    acceptor_ -> setnewconnectionts(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
}

TcpServer::~TcpServer(){
    delete acceptor_;

    for(auto &cn:conns_){
        delete cn.second;//释放所有conn对象
    }
}

void TcpServer::start(){
    evl_.run();
}

void TcpServer::newconnection(Socket* clientsock){
    Connection *conn = new Connection(&evl_,clientsock); //此处对象暂未释放处理
    printf("accept 客户端(fd=%d,ip=%s,port=%d) 成功。\n",conn -> getfd(), conn -> getip().c_str(), conn -> getport());

    conns_[conn->getfd()] = conn;
}