#include "BankServer.h"



BankServer::BankServer(const std::string &ip, const uint16_t port, int subthreadnum, int workthreadnum):ts_(ip,port,subthreadnum),threadpool_(workthreadnum,"WORK"){
    ts_.setnewconnectioncb(std::bind(&BankServer::handlenewconnection,this,std::placeholders::_1));    
    ts_.setcloseconnectioncb(std::bind(&BankServer::handleclose,this,std::placeholders::_1));
    ts_.seterrorconnectioncb(std::bind(&BankServer::handleerror,this,std::placeholders::_1));
    ts_.setonmessagecb(std::bind(&BankServer::handlemessage,this,std::placeholders::_1,std::placeholders::_2));
    ts_.setsendcompletecb(std::bind(&BankServer::handlesendcomplete,this,std::placeholders::_1));
    ts_.settimeoutcb(std::bind(&BankServer::handletimeout, this, std::placeholders::_1));    
    ts_.setremoveconnectioncb(std::bind(&BankServer::handleremove, this, std::placeholders::_1));
}

BankServer::~BankServer(){

}

void BankServer::start(){
    ts_.start();
}

void BankServer::stop(){
    //停止工作线程
    threadpool_.stopthread();
    printf("工作线程已停止\n");

    //停止IO线程
    ts_.stop();
    printf("IO线程池已停止\n");
}

void BankServer::handlenewconnection(spConnection conn){

    //客户端连上时将用户信息保存到状态机中
    spUserInfo userinfo(new UserInfo(conn->getfd(),conn->getip()));
    {
        std::lock_guard<std::mutex> gd(mutex_);
        usermap_[conn->getfd()] = userinfo;     //把用户添加到状态机中
    }

    printf("%s 新建连接(ip = %s).\n",Timestamp::now().tostring().c_str(), conn->getip().c_str());

} 

void BankServer::handleclose(spConnection conn){
    printf("%s 连接已断开(ip=%s).\n", Timestamp::now().tostring().c_str(),conn->getip().c_str());
    {
        std::lock_guard<std::mutex> gd(mutex_);
        usermap_.erase(conn->getfd());  //从状态机中删除用户信息

    }
} 

void BankServer::handleerror(spConnection conn){
    handleclose(conn);

} 

void BankServer::handlemessage(spConnection conn,std::string &message){
    //printf("EchoServer::handlemessage() 线程:%d.\n", syscall(SYS_gettid));
    message = "结果:" + message;

    if(threadpool_.size() == 0){
        //工作线程为0，即没有工作线程
        //直接在IO线程进行计算
        onmessage(conn,message);
    }
    else{
        //业务添加到线程池任务队列中
        threadpool_.addtask(std::bind(&BankServer::onmessage, this, conn, message));
    }
    

}

bool getxmlbuffer(const std::string &xmlbuffer,const std::string &fieldname, std::string &value,const int ilen = 0){
    std::string start = "<" + fieldname + ">";  //数据项的开始标签
    std::string end = "</" + fieldname + ">";   //数据项的结束标签

    int startp = xmlbuffer.find(start);
    if (startp == std::string::npos) return false;

    int endp = xmlbuffer.find(end);
    if(endp == std::string::npos) return false;

    //从xml中截取数据项的内容
    int itmplen = endp - startp - start.length();
    if ((ilen > 0 )&& (ilen < itmplen)) itmplen = ilen;
    value = xmlbuffer.substr(startp+start.length(),itmplen);

    return true;
}

void BankServer::onmessage(spConnection conn, std::string &message){
    spUserInfo userinfo = usermap_[conn->getfd()];

    //解析客户端的请求报文，处理业务
    std::string bizcode;    //业务代码
    std::string replaymessage; //回应报文
    getxmlbuffer(message,"bizcode",bizcode);
    
    if(bizcode == "00101"){//登录业务
        std::string username,password;
        getxmlbuffer(message,"username",username);  //解析用户名
        getxmlbuffer(message,"password",password);  //解析密码
        if((username == "whtbear" && (password == "123456"))){
            //这里可以查询数据库
            replaymessage = "<bizcode>00102</bizcode><retcode>0</retcode><message>ok</message>";
            userinfo -> setLogin(true);
        }
        else{
            replaymessage = "<bizcode>00102</bizcode><retcode>-1</retcode><message>Login failed</message>";
        }
    }
    else if(bizcode=="00001"){
        if(userinfo->isLogin() == true){
            replaymessage="<bizcode>00002</bizcode><retcode>0</retcode><message>ok</message>";
        }
        else{
            replaymessage="<bizcode>00002</bizcode><retcode>-1</retcode><message>User unlogin.</message>";
        }
    }
    else if(bizcode=="00201"){  //查询余额业务
        if(userinfo->isLogin() == true){
            replaymessage="<bizcode>00202</bizcode><retcode>0</retcode><message>2000.30</message>";
        }
        else{
            replaymessage="<bizcode>00202</bizcode><retcode>-1</retcode><message>User unlogin.</message>";
        }
        
    }
    else if (bizcode=="00901"){
        if(userinfo->isLogin() == true){
            replaymessage="<bizcode>00902</bizcode><retcode>0</retcode><message>ok</message>";
        }
        else{
            replaymessage="<bizcode>00902</bizcode><retcode>-1</retcode><message>User unlogin.</message>";
        }
    }

    conn -> send(replaymessage.data(),replaymessage.size());

}

void BankServer::handlesendcomplete(spConnection conn){
    //std::cout<<"数据发送完成"<<std::endl;
}   

void BankServer::handletimeout(EventLoop* evl){
    //std::cout<<"回显服务器超时"<< std::endl;
}  

void BankServer::handleremove(int fd){
    printf("fd(%d) timeout.\n", fd);
    std::lock_guard<std::mutex> gd(mutex_);

    usermap_.erase(fd); //map容器中删除用户状态机

}

