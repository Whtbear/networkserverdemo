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
    clientchannel_ -> setreadcallback(std::bind(&Connection::onmessage,this));
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
    errorcallback_(this);

}   

void Connection::errorcallback(){
    errorcallback_(this);

}   

void Connection::setclosecallback(std::function<void(Connection *)> fn){
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(Connection *)> fn){
    errorcallback_ = fn;
}

void Connection::onmessage(){
    char buffer[1024];//定义缓冲区为2^10个字节
    while(true){
        //非阻塞IO事件循环处理
        memset(&buffer,0,sizeof(buffer));//初始化
        ssize_t nread = recv(getfd(), buffer, sizeof(buffer), 0);
        //buffer是指针首地址，而&buffer是数组首地址
        //flag默认为0
        if(nread > 0){//成功读取到数据

           //printf("recv(eventfd=%d):%s\n",getfd(),buffer);
            //send(getfd(),buffer,strlen(buffer), 0);//返回相同数据
            //先放入接收缓冲区
            inputbuffer_.append(buffer,nread);//接收数据大小
        }
        else if(nread == -1 && errno == EINTR){//读数据被信号中断，继续循环读取
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || errno == EWOULDBLOCK)){
            //数据已读取完毕
            //errno码表示资源短缺不可用或操作将会被阻塞，一个意思。
            printf("recv(eventfd=%d):%s\n",getfd(),inputbuffer_.data());
            // 在这里，经过计算。
            outputbuffer_ = inputbuffer_;
            //将结果放入outputbuffer_中
            inputbuffer_.clear();
            send(getfd(),outputbuffer_.data(),outputbuffer_.size(),0);
            break;
        }
        else if (nread == 0){
            //客户端连接已断开
            closecallback();
            break;
        }
    }
}
