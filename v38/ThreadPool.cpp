#include "ThreadPool.h"

/*
class ThreadPool{
private:
    std::vector<std::thread> threads_;  //线程池中的线程
    std::queue<std::function<void()>> taskqueue_;   //任务队列
    std::mutex mutex_;  //任务队列同步互斥锁
    std::condition_variable condition_; //任务队列同步的条件变量
    std::atomic_bool stop_; //析构函数中stop_值设为true时，全部线程退出


public:
    // 在构造函数中启动threadnum个线程
    ThreadPool(size_t threadnum);
    //任务添加到队列
    void addtask(std::function<void()> task);

    //在析构函数中停止线程
    ~ThreadPool();

};
*/

ThreadPool::ThreadPool(size_t threadnum, const std::string& threadtype):stop_(false),threadtype_(threadtype){
    //启动threadnum个线程
    for(size_t i = 0; i < threadnum; i++){
        //Lambda函数创建线程
        threads_.emplace_back([this]{
            printf("创建 %s 线程(%d).\n",threadtype_.c_str() ,syscall(SYS_gettid));//线程类型、线程编号

            while( stop_ == false){
                //线程池处于工作状态
                std::function<void()> task; //存放出队元素

                {
                    //这里是互斥锁
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    //等待生产者的条件变量
                    this->condition_.wait(lock, [this]{
                        return ((this->stop_ == true) || (this->taskqueue_.empty() == false));
                    });

                    //线程池如果停止，则需要先把队列中的任务执行完再退出
                    if(this->stop_ == true && this->taskqueue_.empty() == true) return;

                    task = std::move(this->taskqueue_.front());//这里用move，避免拷贝
                    this->taskqueue_.pop();
                    
                }

                //printf("%s(%d): 执行任务.\n",threadtype_.c_str() , syscall(SYS_gettid));
                task(); //执行任务
            }
        });
    }
}

void ThreadPool::addtask(std::function<void()> task){
    {   //上锁
        std::lock_guard<std::mutex> lock(mutex_);
        //将任务放入队列
        taskqueue_.push(task);
    }
    
    condition_.notify_one();//唤醒一个线程
}

void ThreadPool::stopthread(){
    if(stop_) return ;
    stop_ = true;

    condition_.notify_all();    //唤醒全部线程

    for(std::thread &th : threads_)
        th.join();

}

ThreadPool::~ThreadPool(){
    stopthread();
}

size_t ThreadPool::size(){
    return threads_.size();
}

/*
void show(int no){
    printf("这是第%d号任务\n",no);
}

void test(){
    printf("这是测试内容\n");
}

int main(){
    ThreadPool threadpool(3);

    threadpool.addtask(std::bind(show,5));
    sleep(1);
    threadpool.addtask(std::bind(test));
    threadpool.addtask(std::bind([]{
        for(int i = 0;i<4;i++){
            printf("输出第%d次\n",i);
        }
    }));
    sleep(1);


}
*/