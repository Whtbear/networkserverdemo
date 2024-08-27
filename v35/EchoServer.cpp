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

void EchoServer::stop(){
    //停止工作线程
    threadpool_.stopthread();
    printf("工作线程已停止\n");

    //停止IO线程
    ts_.stop();
    printf("IO线程池已停止\n");
}

void EchoServer::handlenewconnection(spConnection conn){
    //std::cout << "新的客户端连接"<< std::endl;
    //printf("EchoServer::HandleNewConnection() thread is %d.\n", syscall(SYS_gettid));
    printf("新的连接(fd=%d,ip=%s,port=%d) 已创建。\n",conn->getfd(),conn->getip().c_str(),conn->getport());
} 

void EchoServer::handleclose(spConnection conn){
    //std::cout <<"连接关闭" << std::endl;
    printf("连接(fd=%d,ip=%s,port=%d)关闭。\n",conn->getfd(),conn->getip().c_str(),conn->getport());
} 

void EchoServer::handleerror(spConnection conn){
    //std::cout<<"回显连接出错"<<std::endl;
} 

void EchoServer::handlemessage(spConnection conn,std::string &message){
    //printf("EchoServer::handlemessage() 线程:%d.\n", syscall(SYS_gettid));
    message = "结果:" + message;

    if(threadpool_.size() == 0){
        //工作线程为0，即没有工作线程
        //直接在IO线程进行计算
        onmessage(conn,message);
    }
    else{
        //业务添加到线程池任务队列中
        threadpool_.addtask(std::bind(&EchoServer::onmessage, this, conn, message));
    }
    

}

void EchoServer::onmessage(spConnection conn, std::string &message){
    message = "回复:" + message;
    conn -> send(message.data(),message.size());

}

void EchoServer::handlesendcomplete(spConnection conn){
    //std::cout<<"数据发送完成"<<std::endl;
}   

void EchoServer::handletimeout(EventLoop* evl){
    //std::cout<<"回显服务器超时"<< std::endl;
}  

