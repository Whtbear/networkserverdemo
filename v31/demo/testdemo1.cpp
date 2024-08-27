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
#include <sys/eventfd.h>     // eventfd需要包含这个头文件

int main(int argc,char *argv[]){
    int efd = eventfd(2,0); //创建eventfd
    //三个标志位EFD_CLOEXEC|EFD_NONBLOCK|EFD_SEMAPHORE
    //EFD_CLOEXEC 文件描述符在执行 exec 系列函数（例如 execve）时自动关闭
    //EFD_NONBLOCK 文件描述符以非阻塞模式操作
    //EFDSEMAPHORE 在这种模式下，每次 eventfd_read 操作将减少计数器的值1而不是将其设置为0（默认行为），这允许更精细的控制和同步机制


    uint64_t buf = 3;
    ssize_t ret;
    //eventfd的size大小必须为8字节
    ret = write(efd, &buf, sizeof(uint64_t));
    ret = write(efd, &buf, sizeof(uint64_t));
    //写操作把buf累加，而读操作是一次读完

    ret = read(efd, &buf, sizeof(uint64_t));
    printf("ret = %d,buf = %d\n",ret,buf);

    close(efd); //结束关闭efd

    return 0;
}

