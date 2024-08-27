//注意包含头文件，epoll模型服务端
#include <stdio.h>
#include <signal.h>
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
#include "BankServer.h"

BankServer *bs;

void S_stop(int sig){//信号2和15的处理函数，用于停止服务程序
    printf("sig=%d\n",sig);
    bs -> stop();
    printf("echoserver已停止。\n");
    delete bs;
    printf("delete echoserver\n");
    exit(0);
}

int main(int argc,char *argv[]){
    if(argc != 3){
        //需要提供服务器ip和端口号
        printf("参数： IP地址  端口号\n");
        printf("举例： ./server 192.168.114.129 5005\n\n");
        return -1;
    }

    signal(SIGTERM,S_stop); //信号15，系统kill或killall默认发送的信号
    signal(SIGINT,S_stop);  //信号2，按ctrl+c发送的信号
    //TcpServer ts(argv[1], atoi(argv[2]));
    //ts.start();     //启动事件循环

    bs = new BankServer(argv[1],atoi(argv[2]),3, 2);
    bs->start();

    return 0;
}

