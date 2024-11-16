#include "ThreadPool.h"
#include "Log.h"
#include "WorkerThread.h"
#include <cassert>
#include <pthread.h>
#include <thread>
#include <vector>

ThreadPool::ThreadPool(EventLoop *mainLoop, int count) {
  m_index = 0;
  m_isStart = false;
  m_mainLoop = mainLoop;
  m_threadNum = count;
  m_workerThreads.clear();
}
void ThreadPool::run() {
  assert(!m_isStart);
  if (m_mainLoop->getThreadId() != std::this_thread::get_id()) {
    Debug(("不是主线程启动的线程池"));
    exit(0);
  }
  m_isStart = true;
  if (m_threadNum > 0) {
    for (int i = 0; i < m_threadNum; i++) {
      WorkerThread *subThread = new WorkerThread(i);
      subThread->run();
      m_workerThreads.push_back(subThread);
    }
    Debug("workerThreads赋值");
  }
}

EventLoop *ThreadPool::takeWorkerEventLoop() {
  assert(m_isStart);
  if (m_mainLoop->getThreadId() != std::this_thread::get_id()) {
    exit(0);
  }
  // 从线程池中找一个子线程取出里面的反应堆实例
  // 如果没有子线程，就用主线程的反应堆
  EventLoop *evLoop = m_mainLoop;
  if (m_threadNum > 0) {
    evLoop = m_workerThreads[m_index]->getEventLoop();
    m_index = ++m_index % m_threadNum;
  }
  return evLoop;
}

ThreadPool::~ThreadPool() {
  for (auto item : m_workerThreads) {
    delete item;
  }
}
