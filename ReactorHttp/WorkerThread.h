#pragma once
#include "EventLoop.h"
#include <condition_variable>
#include <mutex>
#include <thread>
class WorkerThread {
public:
  // 初始化
  WorkerThread(int index);
  ~WorkerThread();
  // 启动线程
  void run();
  // 子线程的回调函数
  void running();

  inline EventLoop *getEventLoop() { return m_evLoop; }

private:
  EventLoop *m_evLoop;
  std::thread::id m_theadID;
  std::thread *m_thread;
  std::string name;
  std::mutex m_mutex;
  std::condition_variable m_cond;
};
