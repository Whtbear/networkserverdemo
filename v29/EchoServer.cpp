#include "EchoServer.h"



EchoServer::EchoServer(const std::string &ip, const uint16_t port, int subthreadnum, int workthreadnum):ts_(ip,port,subthreadnum),threadpool_(workthreadnum,"WORK"){
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

void EchoServer::handlenewconnection(spConnection conn){
    std::cout << "新的客户端连接"<< std::endl;
    //printf("EchoServer::HandleNewConnection() thread is %d.\n", syscall(SYS_gettid));

} 

void EchoServer::handleclose(spConnection conn){
    std::cout <<"连接关闭" << std::endl;
} 

void EchoServer::handleerror(spConnection conn){
    std::cout<<"回显连接出错"<<std::endl;
} 

void EchoServer::handlemessage(spConnection conn,std::string &message){
    //printf("EchoServer::handlemessage() 线程:%d.\n", syscall(SYS_gettid));
    message = "结果:" + message;
    sleep(2);
    conn -> send(message.data(),message.size());//发送数据

    //业务添加到线程池任务队列中
    threadpool_.addtask(std::bind(&EchoServer::onmessage, this, conn, message));
}

void EchoServer::onmessage(spConnection conn, std::string &message){
    message = "回复:" + message;
    conn -> send(message.data(),message.size());

}

void EchoServer::handlesendcomplete(spConnection conn){
    std::cout<<"数据发送完成"<<std::endl;
}   

void EchoServer::handletimeout(EventLoop* evl){
    std::cout<<"回显服务器超时"<< std::endl;
}  

