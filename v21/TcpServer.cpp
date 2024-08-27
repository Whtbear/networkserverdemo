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
    //绑定回调函数
    conn -> setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn -> seterrorcallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn -> setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    printf("accept 客户端(fd=%d,ip=%s,port=%d) 成功。\n",conn -> getfd(), conn -> getip().c_str(), conn -> getport());

    conns_[conn->getfd()] = conn;
}

void TcpServer::closeconnection(Connection *conn){
    printf("客户端(eventfd=%d) 关闭连接。\n", conn -> getfd());
    //从map容器中删除对应fd的conn对象
    conns_.erase(conn->getfd());
    delete conn;

} 

void TcpServer::errorconnection(Connection *conn){
    printf("客户端(eventfd=%d) 错误。\n",conn -> getfd());
    conns_.erase(conn->getfd());
    delete conn;

} 

void TcpServer::onmessage(Connection *conn,std::string message){
    message = "结果:" + message;

    int len = message.size();
    std::string tmpbuf((char*)&len, 4);
    tmpbuf.append(message);

    //send(conn->getfd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn -> send(tmpbuf.data(),tmpbuf.size());
}

