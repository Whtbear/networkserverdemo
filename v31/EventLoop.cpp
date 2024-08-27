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

EventLoop::EventLoop():ep_(new Epoll){

} 

EventLoop::~EventLoop(){


}  

void EventLoop::run(){
    printf("线程id:%d.\n", syscall(SYS_gettid));
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

