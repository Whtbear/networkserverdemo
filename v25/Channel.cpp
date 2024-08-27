#include "Channel.h"
/*
class Channel{
private:
    int fd_ = -1; //Channel  与   fd 1:1
    Epoll *ep_ = nullptr;   //红黑树   Channel与Epoll  n:1  而一个Channel只对应一个Epoll
    bool inepoll_ = false;  //Channel是否已被添加到epoll的红黑树上，false则在调用epoll_ctl()时用EPOLL_CTL_ADD
    //否则用EPOLL_CTL_MOD
    uint32_t events_ = 0;   //fd_需要监视的事件，listenfd和clientfd需要监视EPOLLIN，而clientfd还可能需要监视EPOLLOUT
    uint32_t revents_ = 0;  //fd_已发生的事件

public:
    Channel(Epoll *ep,int fd); //构造函数
    ~Channel();     //析构函数

    int getfd();        //返回fd_成员
    void setet();        //切换成边缘触发模式
    void enablereading();   //让epoll_wait()监视fd_的读事件
    void setinepoll();  //设置inepoll的成员值为true
    void setrevents(uint32_t ev);   //设置revents_的成员的值为ev
    bool getinepoll();  
    uint32_t getevents();
    uint32_t getrevents();
};
*/

Channel::Channel(EventLoop *evl,int fd):evl_(evl),fd_(fd){

}

Channel::~Channel(){
    //  Channel的关闭与ep和fd无关
}

int Channel::getfd(){
    return fd_;
}

void Channel::setet(){
    events_ = events_ | EPOLLET;
}

void Channel::enablereading(){
    //监视读事件
    events_ |= EPOLLIN;
    //ep_ -> updatechannel(this);
    evl_ -> updatechannel(this);
}

void Channel::setinepoll(){
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev){
    revents_ = ev;
}

bool Channel::getinepoll(){
    return inepoll_;
}

uint32_t Channel::getevents(){
    return events_;
}

uint32_t Channel::getrevents(){
    return revents_;
}

void Channel::handleevent(){
    if(revents_ & EPOLLRDHUP){
        printf("EPOLLRDHUP\n");
        closecallback_();
    }
    else if(revents_ & (EPOLLIN|EPOLLPRI)){//接收缓冲区存在数据可以读
        //EPOLLPRI表示带外数据（传输紧急数据的方法）
        printf("EPOLLIN|EPOLLPRI\n");
        readcallback_();

    }
    else if(revents_ & EPOLLOUT){//有数据可写，暂时空着
        printf("EPOLLOUT\n");
        writecallback_();
    }
    else{//错误
        errorcallback_();
    }
}

void Channel::setreadcallback(std::function<void()> fn){
    readcallback_ = fn;

}
//设置关闭fd_的回调函数
void Channel::setclosecallback(std::function<void()> fn){
    closecallback_ = fn;
}
//设置fd_发生了错误的回调函数
void Channel::seterrorcallback(std::function<void()> fn){
    errorcallback_ = fn;
}

void Channel::disablereading(){
    events_&=~EPOLLIN;
    evl_->updatechannel(this);
}  
void Channel::enablewriting(){
    events_|=EPOLLOUT;
    evl_->updatechannel(this);
}   
void Channel::disablewriting(){
    events_&=~EPOLLOUT;
    evl_->updatechannel(this);
}  
//设置写事件的回调函数
void Channel::setwritecallback(std::function<void()> fn){
    writecallback_ = fn;
}



