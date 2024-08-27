#include "Channel.h"

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
        printf("客户端(eventfd=%d)关闭连接。\n",fd_);
        close(fd_);
    }
    else if(revents_ & (EPOLLIN|EPOLLPRI)){//接收缓冲区存在数据可以读
        //EPOLLPRI表示带外数据（传输紧急数据的方法）
        readcallback_();

    }
    else if(revents_ & EPOLLOUT){//有数据可写，暂时空着

    }
    else{//错误
        printf("客户端(eventfd=%d) 错误。\n",fd_);
        //关闭客户端
        close(fd_);
    }
}

void Channel::newconnection(Socket *servsock){
    InetAddress clientaddr;  //封装了客户端地址和协议

    //clientsock必须new，在栈上的话析构函数会在本段结束时关闭fd
    //new出来的对象没有释放

    Socket *clientsock = new Socket(servsock->accept(clientaddr));

    printf("accept 客户端(fd=%d,ip=%s,port=%d) 成功。\n",clientsock -> getfd(), clientaddr.getip(),clientaddr.getport());

    //封装简化
    Channel *clientchannel = new Channel(evl_, clientsock -> getfd());
    clientchannel->setreadcallback(std::bind(&Channel::onmessage,clientchannel));
    clientchannel -> enablereading();
    clientchannel -> setet();//改为边缘触发模式
}

void Channel::onmessage(){
    char buffer[1024];//定义缓冲区为2^10个字节
    while(true){
        //非阻塞IO事件循环处理
        memset(&buffer,0,sizeof(buffer));//初始化
        ssize_t nread = recv(fd_, buffer, sizeof(buffer), 0);
        //buffer是指针首地址，而&buffer是数组首地址
        //flag默认为0
        if(nread > 0){//成功读取到数据

            printf("recv(eventfd=%d):%s\n",fd_,buffer);
            send(fd_,buffer,strlen(buffer), 0);//返回相同数据

        }
        else if(nread == -1 && errno == EINTR){//读数据被信号中断，继续循环读取
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || errno == EWOULDBLOCK)){
            //数据已读取完毕
            //errno码表示资源短缺不可用或操作将会被阻塞，一个意思。
            break;
        }
        else if (nread == 0){
            //客户端连接已断开
            printf("客户端(eventfd=%d) 关闭连接。\n", fd_);
            close(fd_);
            break;
        }
    }
}

void Channel::setreadcallback(std::function<void()> fn){
    readcallback_ = fn;

}