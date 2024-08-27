#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <sys/syscall.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

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