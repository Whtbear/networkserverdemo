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
#include "Socket.h"
#include "Epoll.h"


int main(int argc,char *argv[]){
    if(argc != 3){
        //需要提供服务器ip和端口号
        printf("参数： IP地址  端口号\n");
        printf("举例： ./tcpepoll 192.168.114.129 5005\n\n");
        return -1;
    }

    Socket servsock(create_nonblocking());//创建服务端的监听sock

    InetAddress servaddr(argv[1],atoi(argv[2]));//创建结构地址
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true);
    servsock.bind(servaddr);
    servsock.listen();  //默认参数


/*
    int epollfd = epoll_create(2); //创建epoll句柄

    //定义epoll_event结构体
    struct epoll_event ev;  //事件数据结构
    ev.data.fd = servsock.getfd();  //指定事件的自定义数据，随着epoll_wait()返回的事件返回
    ev.events = EPOLLIN;  //监视listenfd读事件，缺省采用水平触发

    //放入epollfd句柄中
    epoll_ctl(epollfd,EPOLL_CTL_ADD,servsock.getfd(),&ev);

    
    struct epoll_event evs[10];//创建数组，存放epoll_wait()返回事件
*/
    Epoll epl;
    epl.addfd(servsock.getfd(),EPOLLIN);
    std::vector<epoll_event> evs;

    while(true){

        evs = epl.loopwait();

        //遍历vector
        for(auto &ev:evs){
        //for(int ii = 0;ii<infds;ii++){               
            //客户端关闭
            if(ev.events & EPOLLRDHUP){
                printf("客户端(eventfd=%d)关闭连接。\n",ev.data.fd);
                close(ev.data.fd);
            }
            else if(ev.events & (EPOLLIN|EPOLLPRI)){//接收缓冲区存在数据可以读
                //EPOLLPRI表示带外数据（传输紧急数据的方法）
                if(ev.data.fd == servsock.getfd()){
                    //listenfd有事件，新的客户端连接
                    //首先用结构体accept客户端

                    InetAddress clientaddr;  //封装了客户端地址和协议

                    //clientsock必须new，在栈上的话析构函数会在本段结束时关闭fd
                    //new出来的对象没有释放

                    Socket *clientsock = new Socket(servsock.accept(clientaddr));

                    printf("accept 客户端(fd=%d,ip=%s,port=%d) 成功。\n",clientsock -> getfd(), clientaddr.getip(),clientaddr.getport());
                    /*
                    ev.data.fd = clientsock -> getfd();
                    ev.events = EPOLLIN|EPOLLET; //设置客户端事件为边缘触发
                    epoll_ctl(epollfd,EPOLL_CTL_ADD, clientsock -> getfd(),&ev);
                    */
                    //封装简化
                    epl.addfd(clientsock->getfd(), EPOLLIN|EPOLLET);
                }
                else{//这里是客户端有数据传输的情况
                    char buffer[1024];//定义缓冲区为2^10个字节
                    while(true){
                        //非阻塞IO事件循环处理
                        memset(&buffer,0,sizeof(buffer));//初始化
                        ssize_t nread = recv(ev.data.fd, buffer, sizeof(buffer), 0);
                        //buffer是指针首地址，而&buffer是数组首地址
                        //flag默认为0
                        if(nread > 0){//成功读取到数据

                            printf("recv(eventfd=%d):%s\n",ev.data.fd,buffer);
                            send(ev.data.fd,buffer,strlen(buffer), 0);//返回相同数据

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
                            printf("客户端(eventfd=%d) 关闭连接。\n",ev.data.fd);
                            close(ev.data.fd);
                            break;
                        }
                    }
                    
                }

            }
            else if(ev.events & EPOLLOUT){//有数据可写，暂时空着

            }
            else{//错误
                printf("客户端(eventfd=%d) 错误。\n",ev.data.fd);
                //关闭客户端
                close(ev.data.fd);
            }


        }
        

    }
    return 0;
}

