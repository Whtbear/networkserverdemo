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
#include "TcpServer.h"
#include "EchoServer.h"


int main(int argc,char *argv[]){
    if(argc != 3){
        //需要提供服务器ip和端口号
        printf("参数： IP地址  端口号\n");
        printf("举例： ./tcpepoll 192.168.114.129 5005\n\n");
        return -1;
    }

    //TcpServer ts(argv[1], atoi(argv[2]));
    //ts.start();     //启动事件循环

    EchoServer es(argv[1],atoi(argv[2]));
    es.start();

    return 0;
}

