#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>

int main(int argc,char *argv[]){
    int pipefd[2];  //无名管道，两个fd，管道两端，fd[0]用于读，fd[1]用于写
    if(pipe2(pipefd,0) == -1){
        printf("管道创建失败。\n");
        return -1;
    }
    //从fd[1]写入数据
    write(pipefd[1], "hello world",11);

    ssize_t ret;
    char buf[128] = {0};
    ret = read(pipefd[0], &buf, 5);
    printf("ret=%d,buf=%s\n",ret,buf);

    ret = read(pipefd[0], &buf, sizeof(buf));
    printf("ret=%d,buf=%s\n", ret, buf);

    close(pipefd[0]);close(pipefd[1]);

    return 0;


}