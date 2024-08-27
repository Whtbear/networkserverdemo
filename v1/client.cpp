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

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("参数：客户端IP 端口号\n");
        printf("用例：./client 192.168.114.129 5005\n\n");
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("socket() 连接失败。\n");
        return -1;
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;//协议簇
    servaddr.sin_port = htons(atoi(argv[2]));//端口号
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);//IP地址

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0){
        //连接服务端
        printf("connect(%s:%s) 失败。\n",argv[1],argv[2]);
        close(sockfd);
        return -1;
    }

    printf("连接成功\n");

    for(int ii = 0;ii<100000;ii++){
        //输入先初始化
        memset(buf,0,sizeof(buf));
        printf("please input:");
        scanf("%s",buf);

        if (send(sockfd,buf,strlen(buf),0) <= 0){
            //发送内容
            printf("send() 失败。\n");
            close(sockfd);
            return -1;
        }

        memset(buf, 0 ,sizeof(buf));
        //接收服务端回应
        if(recv(sockfd,buf,sizeof(buf),0) <= 0){
            printf("recv() 失败。\n");
            close(sockfd);
            return -1;
        }

        printf("recv:%s\n" , buf);
    }

}