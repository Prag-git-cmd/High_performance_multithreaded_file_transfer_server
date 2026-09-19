#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include<vector>
#include<thread>
#include<queue>
#include<functional>
#include<mutex>
#include<condition_variable>
using namespace std;

class ThreadPool{
public:
  explicit ThreadPool(size_t numThreads);
  void enqueue(function<void()> task);
  ~ThreadPool();

private:
  vector<thread> workers;
  queue<function<void()>> tasks;
  mutex queueMutex;
  condition_variable condition;
  bool stop;
};

#endif
