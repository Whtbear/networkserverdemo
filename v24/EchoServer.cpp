#include "EchoServer.h"
/*
class EchoServer{
private:
    TcpServer ts_;

public:
    EchoServer(const std::string &ip, const uint16_t port);
    ~EchoServer();

    void start();    //启动服务

    void handlenewconnection(Socket* clientsock); //处理新客户端连接请求，TcpServer类中回调
    void handleclose(Connection *conn); //关闭客户端连接，TcpServer类中回调
    void handleerror(Connection *conn); //客户端连接错误，TcpServer类中回调
    void handlemessage(Connection *conn,std::string message); //处理客户端的请求报文，TcpServer类中回调
    void handlesendcomplete(Connection *conn);    //数据发送完成后，TcpServer类中回调
    void handletimeout(EventLoop* evl);  //epoll_wait超时，TcpServer类中回调
};
*/



EchoServer::EchoServer(const std::string &ip, const uint16_t port):ts_(ip,port){
    ts_.setnewconnectioncb(std::bind(&EchoServer::handlenewconnection,this,std::placeholders::_1));    
    ts_.setcloseconnectioncb(std::bind(&EchoServer::handleclose,this,std::placeholders::_1));
    ts_.seterrorconnectioncb(std::bind(&EchoServer::handleerror,this,std::placeholders::_1));
    ts_.setonmessagecb(std::bind(&EchoServer::handlemessage,this,std::placeholders::_1,std::placeholders::_2));
    ts_.setsendcompletecb(std::bind(&EchoServer::handlesendcomplete,this,std::placeholders::_1));
    ts_.settimeoutcb(std::bind(&EchoServer::handletimeout, this, std::placeholders::_1));    
}

EchoServer::~EchoServer(){

}

void EchoServer::start(){
    ts_.start();
}

void EchoServer::handlenewconnection(Connection *conn){
    std::cout << "新的客户端连接"<< std::endl;
} 

void EchoServer::handleclose(Connection *conn){
    std::cout <<"连接关闭" << std::endl;
} 

void EchoServer::handleerror(Connection *conn){
    std::cout<<"回显连接出错"<<std::endl;
} 

void EchoServer::handlemessage(Connection *conn,std::string &message){
    message = "结果:" + message;

/*
    //报文长度（头部） +报文内容
    int len = message.size();
    std::string tmpbuf((char*)&len, 4);
    tmpbuf.append(message);
*/
    //send(conn->getfd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn -> send(message.data(),message.size());//发送数据
}

void EchoServer::handlesendcomplete(Connection *conn){
    std::cout<<"数据发送完成"<<std::endl;
}   

void EchoServer::handletimeout(EventLoop* evl){
    std::cout<<"回显服务器超时"<< std::endl;
}  

