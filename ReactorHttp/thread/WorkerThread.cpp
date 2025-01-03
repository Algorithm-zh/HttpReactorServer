#include "WorkerThread.h"
#include "Log.h"
#include <algorithm>
#include <cstdio>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

WorkerThread::WorkerThread(int index) {
  m_evLoop = nullptr;
  m_thread = nullptr;
  m_theadID = std::thread::id();
  name = "SubThread-" + std::to_string(index);
}
// 子线程回调函数
void WorkerThread::running() {
  m_mutex.lock();
  // 因为对evLoop这个共享资源操作了
  m_evLoop = new EventLoop(name);
  m_mutex.unlock();
  m_cond.notify_one();
  m_evLoop->run();
}

// 该项目只有主线程才会和子线程进行资源共享，各子线程之间使用的资源是相互独立的
void WorkerThread::run() {
  m_thread = new std::thread(&WorkerThread::running, this);
  // 用条件变量确保EventLoop实例化成功
  std::unique_lock<std::mutex> locker(m_mutex);
  // 对共享资源访问了，所以加互斥锁(主线程访问的)
  while (m_evLoop == nullptr) {
    m_cond.wait(locker);
  }
}
// 这个析构函数不会被调用，因为线程池不会将子线程销毁，只有线程池不存在时它才会销毁
WorkerThread::~WorkerThread() {
  if (m_thread != nullptr) {
    delete m_thread;
  }
}
