#include "WorkerThread.h"
#include <cstdio>
#include <thread>

WorkerThread::WorkerThread(int index) : evLoop(nullptr) {
  sprintf(this->name, "SubThread-%d", index);
}
WorkerThread *WorkerThread::workerThreadInit(int index) {
  return new WorkerThread(index);
}
// 子线程回调函数
// 为啥加参数，因为这是个静态函数，不能直接调成员变量
void WorkerThread::subThreadRunning(void *arg) {
  WorkerThread *thread = (WorkerThread *)arg;
  thread->mutex.lock();
  // 因为对evLoop这个共享资源操作了
  thread->evLoop = EventLoop::eventLoopInit(thread->name);
  thread->mutex.unlock();
  thread->evLoop->eventLoopRun();
}
void WorkerThread::workerThreadRun() {
  mtheadID = std::thread(&subThreadRunning, this);
  // 阻塞主线程
  mtheadID.join();
}
EventLoop *WorkerThread::getEventLoop() { return evLoop; };
