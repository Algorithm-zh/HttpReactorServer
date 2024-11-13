#pragma once
#include "EventLoop.h"
#include <condition_variable>
#include <mutex>
#include <thread>
class WorkerThread {
public:
  // 初始化
  WorkerThread(int index);
  static WorkerThread *workerThreadInit(int index); // index为了起名字用的
  // 启动线程
  void workerThreadRun();
  // 子线程的回调函数
  static void subThreadRunning(void *arg);
  // 取出反应堆
  EventLoop *getEventLoop();

private:
  EventLoop *evLoop;
  std::thread mtheadID;
  char name[24];
  std::mutex mutex;
  std::condition_variable cond;
};
