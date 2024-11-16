#pragma once
#include "EventLoop.h"
#include "WorkerThread.h"
#include <vector>

class ThreadPool {
public:
  ThreadPool(EventLoop *mainLoop, int count);
  ~ThreadPool();
  void run();
  // 取出线程池中某个子线程的反应堆实例
  EventLoop *takeWorkerEventLoop();

private:
  EventLoop *m_mainLoop;
  bool m_isStart;
  int m_threadNum;
  std::vector<WorkerThread *> m_workerThreads;
  int m_index;
};
