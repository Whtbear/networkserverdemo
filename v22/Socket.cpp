#include "Socket.h"
/*
class Socket{

private:
    const int fd_; //Socket的fd
public:
    Socket(int fd); //传入fd
    ~Socket();  //需要关闭fd

    int getfd() const; //返回fd_成员
    void setreuseaddr(bool on); 
    void setreuseport(bool on);    
    void settcpnodelay(bool on);
    void setkeepalive(bool on);  
    void bind(const InetAddress& servaddr);  //服务端socket调用函数进行bind()
    void listen(int qn = 256);  //监听函数
    void accpet(InetAddress& clientaddr); //接收客户端函数

};
*/

int create_nonblocking(){
    //创建非阻塞socket
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenfd < 0){//创建失败
        perror("socket() failed");
        exit(-1);
    }
    return listenfd;
}


Socket::Socket(int fd):fd_(fd){

}

Socket::~Socket(){
    ::close(fd_);
}

std::string Socket::getip() const{
    return ip_;
}  

uint16_t Socket::getport() const{
    return port_;
}


int Socket::getfd() const{
    return fd_;
}

void Socket::setreuseaddr(bool on){
    int optval = on ? 1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&optval, sizeof(optval));
}

void Socket::setreuseport(bool on){
    int optval = on ? 1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&optval, sizeof(optval));
}

void Socket::settcpnodelay(bool on){
    int optval = on ? 1:0;
    ::setsockopt(fd_,SOL_SOCKET,TCP_NODELAY,&optval, sizeof(optval));
}

void Socket::setkeepalive(bool on){
    int optval = on ? 1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&optval, sizeof(optval));
}

void Socket::bind(const InetAddress& servaddr){
    if(::bind(fd_,servaddr.getaddr(),sizeof(sockaddr)) < 0){
        //绑定sockaddr结构体
        perror("bind() failed");
        close(fd_);
        exit(-1);
    }

//    ip_ = servaddr.getip();
//    port_ = servaddr.getport();
    setipnport(servaddr.getip(),servaddr.getport());
}

void Socket::listen(int qn){
    if(::listen(fd_,qn) != 0){
        perror("listen() failed");
        close(fd_);
        exit(-1);
    }
}

void Socket::setipnport(const std::string &ip,uint16_t port){
    ip_ = ip;
    port_ = port;
}

int Socket::accept(InetAddress &clientaddr){
    struct sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    int clientfd = accept4(fd_, (struct sockaddr*)&peeraddr, &len,SOCK_NONBLOCK);

    clientaddr.setaddr(peeraddr);

    return clientfd;
}