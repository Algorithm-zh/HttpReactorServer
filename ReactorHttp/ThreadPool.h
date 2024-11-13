#pragma once
#include "EventLoop.h"
#include "WorkerThread.h"
#include <vector>

class ThreadPool {
public:
  ThreadPool(EventLoop *mainLoop, int count);
  ThreadPool *threadPoolInit(EventLoop *mainLoop, int count);
  void threadPoolRun();
  // 取出线程池中某个子线程的反应堆实例
  EventLoop *takeWorkerEventLoop();

private:
  EventLoop *mainLoop;
  bool isStart;
  int threadNum;
  std::vector<WorkerThread *> workerThreads;
  int index;
};
