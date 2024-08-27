#include "EventLoop.h"

/*
class EventLoop{
private:
    Epoll *ep_; //一个事件循环只能有一个Epoll

public:
    EventLoop(); //构建对象ep_
    ~EventLoop();  //在析构函数中销毁ep_

    void run(); //运行事件循环
};
*/

EventLoop::EventLoop():ep_(new Epoll),wakeefd_(eventfd(0,EFD_NONBLOCK)),wakechannel_(new Channel(this, wakeefd_)){
    wakechannel_ -> setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakechannel_ -> enablereading();
} 

EventLoop::~EventLoop(){


}  

void EventLoop::run(){
    //printf("线程id:%d.\n", syscall(SYS_gettid));
    threadid_ = syscall(SYS_gettid);    //获取事件循环所在线程的id，运行事件时获取
    while(true){
        std::vector<Channel *> channels = ep_ -> loopwait();

        //遍历vector
        //如果channels为空，则epoll_wait()超时，回调TcpServer
        if(channels.size() == 0){
            epolltimeoutcallback_(this);
        }
        else{
            for(auto &ch:channels){            
                
                ch -> handleevent();

            }
        }

    }
} 

//把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
void EventLoop::updatechannel(Channel *ch){
    ep_ -> updatechannel(ch);

}

void EventLoop::removechannel(Channel *ch){
    ep_ -> removechannel(ch);

}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn){
    epolltimeoutcallback_ = fn;
}

bool EventLoop::isloopthread(){
    return threadid_ == syscall(SYS_gettid);
}

void EventLoop::queueinloop(std::function<void()> fn){
    {
        //加锁
        std::lock_guard<std::mutex> gd(mutex_); //给任务队列加锁
        taskqueue_.push(fn);    //任务入队
    }

    //唤醒事件循环
    wakeup();
}

void EventLoop::wakeup(){
    uint64_t val = 1;
    write(wakeefd_,&val,sizeof(val));
}

void EventLoop::handlewakeup(){
    printf("handlewakeup() 线程id为%d.\n", syscall(SYS_gettid));

    uint64_t val;
    read(wakeefd_,&val,sizeof(val));

    std::function<void()> fn;

    std::lock_guard<std::mutex> gd(mutex_); //给任务队列加锁

    while(taskqueue_.size() > 0){
        fn = std::move(taskqueue_.front()); //出队元素
        taskqueue_.pop();
        fn();   //执行任务队列中的元素
    }

}