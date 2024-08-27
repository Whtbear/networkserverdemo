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

TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadnum):threadnum_(threadnum),mainevl_(new EventLoop(true))
,acceptor_(mainevl_.get(),ip,port),threadpool_(threadnum_, "IO")
{
    //创建主事件
    
    //mainevl_ = new EventLoop();
    //acceptor_ = new Acceptor(mainevl_,ip,port);
    //设置回调函数
    acceptor_.setnewconnectionts(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    mainevl_ -> setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
    
    //threadpool_ = new ThreadPool(threadnum_, "IO");   //创建线程池
    //创建从事件
    for(int ii = 0; ii < threadnum_; ii++){
        subevls_.emplace_back((new EventLoop(false,5,10)));
        subevls_[ii] -> setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        subevls_[ii] -> settimercallback(std::bind(&TcpServer::removeconn,this,std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subevls_[ii].get()));
    }

}

TcpServer::~TcpServer(){
    //delete acceptor_;
    //delete mainevl_;
    /*  系统自动释放
    for(auto &cn:conns_){
        delete cn.second;//释放所有conn对象
    }
    */
    /*//释放从事件的循环
    for(auto &e:subevls_){
        delete e;
    }
    */

    //delete threadpool_;
}

void TcpServer::start(){
    mainevl_->run();
}

void TcpServer::stop(){
    //停止主事件循环、从事件循环、IO线程
    mainevl_ -> stop();
    printf("主事件循环已停止。\n");

    for(int ii = 0; ii < threadnum_;ii++){
        subevls_[ii] -> stop();
    }
    printf("从事件循环已停止。\n");

    threadpool_.stopthread();
    printf("主事件循环已停止。\n");
}

void TcpServer::newconnection(std::unique_ptr<Socket> clientsock){
    //Connection *conn = new Connection(mainevl_,clientsock); //此处对象暂未释放处理
    spConnection conn(new Connection(subevls_[clientsock->getfd()%threadnum_].get(), std::move(clientsock)));
        
    //绑定回调函数
    conn -> setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn -> seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn -> setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn -> setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
    //printf("accept 客户端(fd=%d,ip=%s,port=%d) 成功。\n",conn -> getfd(), conn -> getip().c_str(), conn -> getport());

    {
        std::lock_guard<std::mutex> gd(evlmutex_);
        conns_[conn->getfd()] = conn;
    }
    subevls_[conn->getfd()%threadnum_] -> newconnection(conn);

    newconnectioncb_(conn);//回调EchoServer::handlenewconnection();

}

void TcpServer::closeconnection(spConnection conn){
    if(closeconnectioncb_) closeconnectioncb_(conn);

    //printf("客户端(eventfd=%d) 关闭连接。\n", conn -> getfd());
    //从map容器中删除对应fd的conn对象
    {
        std::lock_guard<std::mutex> gd(evlmutex_);
        conns_.erase(conn->getfd());
    }

} 

void TcpServer::errorconnection(spConnection conn){
    //std::cout<<"标记"<<std::endl;
    if(errorconnectioncb_) errorconnectioncb_(conn);
    //printf("客户端(eventfd=%d) 错误。\n",conn -> getfd());
    {
        std::lock_guard<std::mutex> gd(evlmutex_);
        conns_.erase(conn->getfd());
    }
} 

void TcpServer::onmessage(spConnection conn,std::string &message){
    if(onmessagecb_) onmessagecb_(conn,message);
}

void TcpServer::sendcomplete(spConnection conn){

    if(sendcompletecb_) sendcompletecb_(conn);//此处EchoServer类中的回调函数处理业务
}

void TcpServer::epolltimeout(EventLoop* evl){
    if(timeoutcb_) timeoutcb_(evl);    

}


void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn){
    newconnectioncb_ = fn;
}
void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn){
    closeconnectioncb_ = fn;
}
void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn){
    errorconnectioncb_ = fn;
}
void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &message)> fn){
    onmessagecb_ = fn;
}
void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn){
    sendcompletecb_ = fn;
}
void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn){
    timeoutcb_ = fn;
}

void TcpServer::removeconn(int fd){
    {
        std::lock_guard<std::mutex> gd(evlmutex_);
        conns_.erase(fd);
    }
}