//注意包含头文件，epoll模型服务端
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  //错误代码
#include <sys/socket.h>  // socket
#include <sys/types.h>   
#include <arpa/inet.h>
#include <sys/fcntl.h>  //文件描述符控制 阻塞与非阻塞
#include <sys/epoll.h>  //epoll模型
#include <netinet/tcp.h>    //TCP_NODELAY 禁用Nagle算法
#include "InetAddress.h"

int main(int argc,char *argv[]){
    if(argc != 3){
        //需要提供服务器ip和端口号
        printf("参数： IP地址  端口号\n");
        printf("举例： ./tcpepoll 192.168.114.129 5005\n\n");
        return -1;
    }

    //创建监听listenfd
    int listenfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    //这里非阻塞状态可以直接设置，不用通过fcntl函数设置。

    if(listenfd < 0){//创建失败
        perror("socket() failed");
        return -1;
    }

    //设置监听listenfd的属性
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,&opt, static_cast<socklen_t>(sizeof opt));
    //设置SO_REUSEADDR可以让该端口正在使用而处于 TIME_WAIT 状态时，允许新的套接字绑定到同一端口。
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY,&opt, static_cast<socklen_t>(sizeof opt));
    //禁用 Nagle 算法
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT,&opt, static_cast<socklen_t>(sizeof opt));
    //允许多个进程或线程在同一个端口上进行监听，从而实现负载均衡。这对于多核系统上的服务器程序非常有用，可以提高网络应用程序的性能和吞吐量。
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE,&opt, static_cast<socklen_t>(sizeof opt));
    //使得套接字在连接空闲时定期发送保持活动探测包，以确认连接的有效性，从而在连接失效时及时关闭连接。

    //setnonblocking(listenfd);//监听fd设置为非阻塞
    //在创建listenfd时已经设置过

    //这里用结构体封装类
    InetAddress servaddr(argv[1],atoi(argv[2]));

    if(bind(listenfd,servaddr.getaddr(),sizeof(sockaddr)) < 0){
        //绑定sockaddr结构体
        perror("bind() failed");
        close(listenfd);
        return -1;
    }

    //要先将listenfd置入监听状态，再放入epoll中
    if(listen(listenfd, 128) != 0){
        perror("listen() 失败");
        close(listenfd);
        return -1;
    }

    int epollfd = epoll_create(2); //创建epoll句柄

    //定义epoll_event结构体
    struct epoll_event ev;  //事件数据结构
    ev.data.fd = listenfd;  //指定事件的自定义数据，随着epoll_wait()返回的事件返回
    ev.events = EPOLLIN;  //监视listenfd读事件，缺省采用水平触发

    //放入epollfd句柄中
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

    
    struct epoll_event evs[10];//创建数组，存放epoll_wait()返回事件

    while(true){

        int infds = epoll_wait(epollfd,evs,10,-1);//阻塞监视fd，等待有事件发生，将event放入

        if(infds < 0){
            perror("epoll_wait() 函数返回失败");
            break;

        }
        if(infds == 0){//超时情况
            printf("epoll_wait() 函数超时\n");
            continue;
        }   

        //infds大于0的情况，说明有事件发生

        for(int ii = 0;ii<infds;ii++){               
            //客户端关闭
            if(evs[ii].events & EPOLLRDHUP){
                printf("客户端(eventfd=%d)关闭连接。\n",evs[ii].data.fd);
                close(evs[ii].data.fd);
            }
            else if(evs[ii].events & (EPOLLIN|EPOLLPRI)){//接收缓冲区存在数据可以读
                //EPOLLPRI表示带外数据（传输紧急数据的方法）
                if(evs[ii].data.fd == listenfd){
                    //listenfd有事件，新的客户端连接
                    //首先用结构体accept客户端

                    struct sockaddr_in peeraddr;
                    socklen_t len = sizeof(peeraddr);
                    int clientfd = accept4(listenfd, (struct sockaddr*)&peeraddr, &len,SOCK_NONBLOCK);
                    //得到客户端fd后设置为非阻塞，通过accept4函数

                    InetAddress clientaddr(peeraddr);  //封装了客户端地址和协议

                    printf("accept 客户端(fd=%d,ip=%s,port=%d) 成功。\n",clientfd, clientaddr.getip(),clientaddr.getport());
                    ev.data.fd = clientfd;
                    ev.events = EPOLLIN|EPOLLET; //设置客户端事件为边缘触发
                    epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);

                }
                else{//这里是客户端有数据传输的情况
                    char buffer[1024];//定义缓冲区为2^10个字节
                    while(true){
                        //非阻塞IO事件循环处理
                        memset(&buffer,0,sizeof(buffer));//初始化
                        ssize_t nread = recv(evs[ii].data.fd, buffer, sizeof(buffer), 0);
                        //buffer是指针首地址，而&buffer是数组首地址
                        //flag默认为0
                        if(nread > 0){//成功读取到数据

                            printf("recv(eventfd=%d):%s\n",evs[ii].data.fd,buffer);
                            send(evs[ii].data.fd,buffer,strlen(buffer), 0);//返回相同数据

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
                            printf("客户端(eventfd=%d) 关闭连接。\n",evs[ii].data.fd);
                            close(evs[ii].data.fd);
                            break;
                        }
                    }
                    
                }

            }
            else if(evs[ii].events & EPOLLOUT){//有数据可写，暂时空着

            }
            else{//错误
                printf("客户端(eventfd=%d) 错误。\n",evs[ii].data.fd);
                //关闭客户端
                close(evs[ii].data.fd);
            }


        }
        

    }
    return 0;
}

