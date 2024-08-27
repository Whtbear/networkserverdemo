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
    clientchannel_ -> setwritecallback(std::bind(&Connection::writecallback,this));
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
            //printf("recv(eventfd=%d):%s\n",getfd(),inputbuffer_.data());
            while(true){
                int len;
                memcpy(&len,inputbuffer_.data(),4);
                if(inputbuffer_.size() < len + 4) break;//报文内容不完整
                
                std::string message(inputbuffer_.data()+4,len);
                inputbuffer_.erase(0, len + 4);

                printf("message (eventfd=%d):%s\n", getfd(), message.c_str());
                
                onmessagecallback_(this, message);  //回调TcpServer::onmessage()
                
            }
            break;
        }
        else if (nread == 0){
            //客户端连接已断开
            closecallback();
            break;
        }
    }
}

//设置处理报文的回调函数
void Connection::setonmessagecallback(std::function<void(Connection *, std::string)> fn){
    onmessagecallback_ = fn;
}

void Connection::send(const char *data,size_t size){
    outputbuffer_.append(data, size); //把需要发送的数据保存到Connection的发送缓冲区中
    //注册写事件
    clientchannel_->enablewriting();
    //一旦socket可写，立刻发送数据
    //在边缘触发状态


}


//处理写事件回调函数，由Channel类回调
void Connection::writecallback(){
    int writen = ::send(getfd(), outputbuffer_.data(), outputbuffer_.size(), 0);    //尝试把outputbuffer_中的数据全部发送出去
    if (writen > 0) outputbuffer_.erase(0,writen);  //从 outputbuffer_中删除已成功发送的字节数

    if(outputbuffer_.size() == 0){//数据已全部发送成功，不再关注写事件
        clientchannel_->disablewriting();
    }
}

