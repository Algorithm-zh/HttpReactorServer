#pragma once
#include "ChannelMap.h"
#include "Dispatcher.h"
#include <mutex>
#include <queue>
#include <thread>
class Dispatcher;

// 处理该节点中的channel方式
enum ElemType { ADD, DELETE, MODEIFY };
// 定义任务队列的节点
class ChannelElement {

public:
  int type;
  Channel *channel;
};
class EventLoop {
public:
  EventLoop(const char *name);
  static EventLoop *eventLoopInit(const char *name);
  static EventLoop *eventLoopInitNoName(const char *name);
  // 启动反应堆模型
  int eventLoopRun();
  // 处理激活的文件fd
  int eventActivate(int fd, int event);
  // 添加任务到任务队列
  int eventLoopAddTask(Channel *channel, int type);
  // 写数据
  void taskWakeUp();
  // 读数据
  static void readLocalMessage(void *arg);
  // 处理任务队列中的任务
  int eventLoopProcessTask();
  // 处理dispatcher中的节点
  int eventLoopAdd(Channel *channel);
  int eventLoopRemove(Channel *channel);
  int eventLoopModify(Channel *channel);
  // 释放Channel
  int destoryChannel(Channel *channel);

  int getThreadId();

private:
  bool isQuit;
  Dispatcher *dispatcher;
  // 任务队列（添加删除修改fd都是一个任务，把它添加到任务队列里）
  std::queue<ChannelElement *> mQueue;
  // map
  ChannelMap *mChannelMap;
  // mutex
  std::mutex mutex;
  int threadId;
  const char *threadName;
  // 存储本地通信的fd，通过socketpair初始化
  int msocketpair[2];
};
