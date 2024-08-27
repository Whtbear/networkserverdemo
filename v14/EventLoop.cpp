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
    delete ep_;
}  

void EventLoop::run(){
    while(true){
        std::vector<Channel *> channels = ep_ -> loopwait();

        //遍历vector
        for(auto &ch:channels){            
            //客户端关闭
            ch -> handleevent();

        }
        

    }
} 

//把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
void EventLoop::updatechannel(Channel *ch){
    ep_ -> updatechannel(ch);

}


