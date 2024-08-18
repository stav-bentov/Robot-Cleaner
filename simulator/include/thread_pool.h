#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <vector>
#include <queue>
#include <thread> 
#include <mutex> 
#include <condition_variable> 

// Got help from: https://www.geeksforgeeks.org/thread-pool-in-cpp/
class ThreadPool {
    public:
        ThreadPool(int numTreads);
        ~ThreadPool();
        void addThreadToPull(std::function<void()> work);
        void waitForAll();
    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex threadsVectorMutex;
        std::condition_variable cv;
        bool stop;
};

#endif