#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

int create_nonblocking();

//封装Socket类
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
    //设置参数，  
    /*
        允许新的套接字绑定到同一端口
        Nagle 算法
        允许多个进程或线程在同一个端口上进行监听，从而实现负载均衡
        套接字在连接空闲时定期发送保持活动探测包，以确认连接的有效性
        连接失效时及时关闭连接
    */

    void bind(const InetAddress& servaddr);  //服务端socket调用函数进行bind()
    void listen(int qn = 256);  //监听函数
    int accept(InetAddress& clientaddr); //接收客户端函数，需要返回客户端fd

};