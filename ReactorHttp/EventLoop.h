#pragma once
#include "Channel.h"
#include "Dispatcher.h"
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
class Dispatcher;

// 处理该节点中的channel方式
enum class ElemType : char { ADD, DELETE, MODIFY };
// 定义任务队列的节点
class ChannelElement {

public:
  ElemType type;
  Channel *channel;
};
class EventLoop {
public:
  EventLoop(const std::string threaName);
  EventLoop();
  ~EventLoop();
  // 启动反应堆模型
  int run();
  // 处理激活的文件fd
  int eventActivate(int fd, int event);
  // 添加任务到任务队列
  int addTask(Channel *channel, ElemType type);
  // 读数据
  int readLocalMessage();
  // 处理任务队列中的任务
  int processTaskQ();
  // 处理dispatcher中的节点
  int add(Channel *channel);
  int remove(Channel *channel);
  int modify(Channel *channel);
  // 释放Channel
  int freeChannel(Channel *channel);

  inline std::thread::id getThreadId() { return m_threadId; }
  inline std::string getThreadName() { return m_threadName; };

private:
  bool m_isQuit;
  Dispatcher *m_dispatcher;
  // 任务队列（添加删除修改fd都是一个任务，把它添加到任务队列里）
  std::queue<ChannelElement *> m_taskQ;
  // map
  std::map<int, Channel *> m_channelMap;
  // mutex
  std::mutex m_mutex;
  std::thread::id m_threadId;
  std::string m_threadName;
  // 存储本地通信的fd，通过socketpair初始化
  int m_socketpair[2];

  // 写数据
  void taskWakeUp();
};
