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
  int type;
  Channel *channel;
};
class EventLoop {
public:
  EventLoop();
  static EventLoop *eventLoopInit();

private:
  bool isQuit;
  Dispatcher *dispatcher;
  // 任务队列
  std::queue<ChannelElement *> mQueue;
  // map
  ChannelMap *mChannelMap;
  // mutex
  std::mutex mutex;
};
