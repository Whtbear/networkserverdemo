//网银客户端程序
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

//发送报文，4字节报头
ssize_t tcpsend(int fd,void *data, size_t size){
    char tmpbuf[1024];  //报文头部+报文内容
    memset(tmpbuf,0,sizeof(tmpbuf));
    memcpy(tmpbuf,&size,4); //拼接报文头部
    memcpy(tmpbuf+4,data,size); //拼接报文内容

    return send(fd,tmpbuf,size+4,0);    //请求报文发送给服务器
}

//接收报文，4字节报头
ssize_t tcprecv(int fd,void *data){
    int len;
    recv(fd,&len,4,0);
    return recv(fd,data,len,0);
}




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

    //登录业务
    memset(buf,0,sizeof(buf));
    sprintf(buf,"<bizcode>00101</bizcode><username>whtbear</username><password>123456</password>");
    if(tcpsend(sockfd,buf,strlen(buf)) <= 0){
        printf("tcpsend() failed.\n");
        return -1;
    }
    printf("send: %s\n",buf);

    memset(buf,0,sizeof(buf));//接收
    if(tcprecv(sockfd,buf) <= 0){
        printf("tcprecv failed.\n");
        return -1;
    }
    printf("recv: %s\n",buf);

    //查询余额
    memset(buf,0,sizeof(buf));
    sprintf(buf,"<bizcode>00201</bizcode><cardid>123456789</cardid>");
    if(tcpsend(sockfd,buf,strlen(buf)) <= 0){
        printf("tcpsend() failed.\n");
        return -1;
    }
    printf("send: %s\n",buf);

    memset(buf,0,sizeof(buf));
    if(tcprecv(sockfd,buf) <= 0){
        printf("tcprecv() failed.\n");
        return -1;
    }
    printf("recv: %s\n",buf);
    //心跳，保持连接
    while(true){
        memset(buf,0,sizeof(buf));
        sprintf(buf,"<bizcode>00001</bizcode>");
        if(tcpsend(sockfd,buf,strlen(buf)) <= 0){
            printf("tcpsend() failed.\n");
            return -1;
        }
        printf("send: %s\n",buf);

        memset(buf,0,sizeof(buf));
        if(tcprecv(sockfd,buf) <= 0){
            printf("tcprecv() failed.\n");
            return -1;
        }
        printf("recv: %s\n",buf);
        sleep(5);
    }

    /*
    //注销业务
    memset(buf,0,sizeof(buf));
    sprintf(buf,"<bizcode>00901</bizcode>");
    if(tcpsend(sockfd,buf,strlen(buf)) <= 0){
        printf("tcpsend() failed.\n");
        return -1;
    }
    printf("send: %s\n",buf);

    memset(buf,0,sizeof(buf));
    if(tcprecv(sockfd,buf) <= 0){
        printf("tcprecv() failed.\n");
        return -1;
    }
    printf("recv: %s\n",buf);
    */
}