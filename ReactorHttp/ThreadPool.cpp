#include "ThreadPool.h"
#include "WorkerThread.h"
#include <cassert>
#include <pthread.h>

ThreadPool::ThreadPool(EventLoop *mainLoop, int count) {
  index = 0;
  isStart = false;
  this->mainLoop = mainLoop;
  threadNum = count;
}
ThreadPool *ThreadPool::threadPoolInit(EventLoop *mainLoop, int count) {
  return new ThreadPool(mainLoop, count);
}
void ThreadPool::threadPoolRun() {
  assert(!isStart);
  if (mainLoop->getThreadId() != pthread_self()) {
    exit(0);
  }
  isStart = true;
  if (threadNum) {
    for (int i = 0; i < threadNum; i++) {
      workerThreads[i] = WorkerThread::workerThreadInit(i);
      workerThreads[i]->workerThreadRun();
    }
  }
}

EventLoop *ThreadPool::takeWorkerEventLoop() {
  assert(isStart);
  if (mainLoop->getThreadId() != pthread_self()) {
    exit(0);
  }
  // 从线程池中找一个子线程取出里面的反应堆实例
  // 如果没有子线程，就用主线程的反应堆
  EventLoop *evLoop = mainLoop;
  if (threadNum > 0) {
    evLoop = workerThreads[index]->getEventLoop();
    index = ++index % threadNum;
  }
  return evLoop;
}
