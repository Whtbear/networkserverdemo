#include "Epoll.h"
/*
//Epoll类
class Epoll{
private:
    int epollfd_ = -1;  //epoll句柄
    static const int Max_Events = 100;
    epoll_event events_[Max_Events];
public:
    Epoll();    //构造函数中创建epollfd句柄
    ~Epoll();   //析构函数中关闭epollfd

    void addfd(int fd,uint32_t op);//把fd和需要监视的事件添加到红黑树
    std::vector<epoll_event> loopwait(int timeout=-1);//运行epoll_wait()，等待事件发生
    //发生事件用vector返回

}
*/

Epoll::Epoll(){
    if((epollfd_ = epoll_create(5)) == -1){
        printf("epoll_create() 失败(%d)。\n",errno);
        exit(0);
    }
}

Epoll::~Epoll(){
    close(epollfd_);
}

/*void Epoll::addfd(int fd,uint32_t op){
    //定义epoll_event结构体
    struct epoll_event ev;  //事件数据结构
    ev.data.fd = fd;  //指定事件的自定义数据，随着epoll_wait()返回的事件返回
    ev.events = op;  //监视listenfd读事件，缺省采用水平触发

    //放入epollfd句柄中
    if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1){
        printf("epoll_ctl() 失败(%d)。\n", errno);
        exit(-1);
    }
}*/

void Epoll::updatechannel(Channel *ch){
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->getevents();

    if(ch->getinepoll()){
        //channel已经在树上，更新
        if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->getfd(), &ev) == -1){
            perror("epoll_ctl() failed.\n");
            exit(-1);
        }
    }
    else{
        //不在树上，添加
        if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->getfd(), &ev) == -1){
            perror("epoll_ctl() failed.\n");
            exit(-1);
        }
        ch -> setinepoll(); //把channel的inepoll_成员设置为true
    }

}
/*
std::vector<epoll_event> Epoll::loopwait(int timeout){
    //运行epoll_wait()，等待事件发生
    std::vector<epoll_event> evs;

    memset(events_, 0, sizeof(events_));
    int infds=epoll_wait(epollfd_, events_, Max_Events, timeout);

    //返回失败
    if (infds<0){
        perror("epoll_wait() failed");
        exit(-1);
    }

    if(infds == 0){
        printf("epoll_wait() 超时。\n");
        return evs;
    }

    //遍历发生事件
    for(int ii = 0 ;ii<infds ;ii++){
        evs.push_back(events_[ii]);

    }

    return evs;
}
*/

std::vector<Channel *> Epoll::loopwait(int timeout){
    //运行epoll_wait()，等待事件发生
    std::vector<Channel *> channels;

    memset(events_, 0, sizeof(events_));
    int infds=epoll_wait(epollfd_, events_, Max_Events, timeout);

    //返回失败
    if (infds<0){
        perror("epoll_wait() failed");
        exit(-1);
    }

    if(infds == 0){
        printf("epoll_wait() 超时。\n");
        return channels;
    }

    //遍历发生事件
    for(int ii = 0 ;ii<infds ;ii++){
        //evs.push_back(events_[ii]);
        Channel *ch= (Channel *)events_[ii].data.ptr;
        ch -> setrevents(events_[ii].events);
        channels.push_back(ch);
    }

    return channels;
}

