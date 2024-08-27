//需要用的头文件
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "InetAddress.h"

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("参数:客户端IP 端口号\n");
        printf("用例:./client 192.168.114.129 5005\n\n");
        return -1;
    }

    int sockfd;
    char buf[1024];

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("socket() 连接失败。\n");
        return -1;
    }

    InetAddress servaddr(argv[1],atoi(argv[2]));

    if(connect(sockfd, servaddr.getaddr(), sizeof(sockaddr)) != 0){
        //连接服务端
        printf("connect(%s:%s) 失败。\n",argv[1],argv[2]);
        close(sockfd);
        return -1;
    }

    printf("连接成功\n");

    for(int ii = 0;ii<3;ii++){
        //输入先初始化
        memset(buf,0,sizeof(buf));
        sprintf(buf,"这是输入的第%d个数字。",ii);

        char tmpbuf[1024];  //临时的buffer，报文头部+报文内容        
        memset(tmpbuf,0,sizeof(tmpbuf));
        int len = strlen(buf);  //计算报文的大小
        memcpy(tmpbuf,&len,4);  //拼接报文头部
        memcpy(tmpbuf+4,buf,len);   //拼接报文内容

        send(sockfd,tmpbuf,len+4,0);    //把请求报文发送给服务端
    }
    //发送和接收都一次性的
    for(int ii = 0;ii < 3;ii++){
        int len;
        recv(sockfd,&len,4,0);  //读4字节报文头部

        memset(buf,0,sizeof(buf));
        
        recv(sockfd,buf,len, 0);//读取报文内容


        printf("recv:%s\n" , buf);
    }

}