#include "EventLoop.h"
#include "Channel.h"
#include "ChannelMap.h"
#include "EpollDispatcher.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

// 写数据
// 目的就是往pair[0]写数据以此激活pair[1]
void EventLoop::taskWakeUp() {
  const char *msg = "激活激活";
  write(msocketpair[0], msg, strlen(msg));
}
// 读数据
void EventLoop::readLocalMessage(void *arg) {
  EventLoop *evLoop = (EventLoop *)arg;
  char buf[256];
  read(evLoop->msocketpair[1], buf, sizeof(buf));
}

int EventLoop::destoryChannel(Channel *channel) {
  // 删除channel和fd的对应关系
  mChannelMap->ChannelFdDelete(channel->getFd());
  close(channel->getFd());
  delete channel;
  return 0;
}
int EventLoop::eventLoopAdd(Channel *channel) {
  int fd = channel->getFd();
  mChannelMap->setChannelMap(fd, channel);
  dispatcher->add(channel, this);
  return 0;
}
int EventLoop::eventLoopRemove(Channel *channel) {
  int fd = channel->getFd();
  int ret = dispatcher->remove(channel, this);
  return ret;
}
int EventLoop::eventLoopModify(Channel *channel) {
  int fd = channel->getFd();
  if (mChannelMap->getChannel(fd) == nullptr)
    return -1;
  int ret = dispatcher->modify(channel, this);
  return ret;
}
int EventLoop::eventLoopProcessTask() {
  std::lock_guard<std::mutex> lck(mutex);
  while (!mQueue.emplace()) {
    ChannelElement *ChannelElement = mQueue.front();
    mQueue.pop();
    if (ChannelElement->type == ADD) {
      eventLoopAdd(ChannelElement->channel);
    } else if (ChannelElement->type == MODEIFY) {
      eventLoopModify(ChannelElement->channel);
    } else if (ChannelElement->type == DELETE) {
      eventLoopRemove(ChannelElement->channel);
    }
    delete ChannelElement;
  }

  return 0;
}

EventLoop::EventLoop(const char *name) : isQuit(false) {
  dispatcher = new EpollDispatcher();
  threadId = pthread_self();
  mChannelMap = ChannelMap::ChannelMapInit();
  threadName = name;
  // 两个fd之间本地网络通信
  int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, msocketpair);
  if (ret == -1) {
    perror("socketpair");
    exit(0);
  }
  // 指定规则msocketpair[0] 发送数据，msocketpair[1]接收数据
  Channel *channel = Channel::channelInit(msocketpair[1], ReadEvent,
                                          readLocalMessage, NULL, this);
  // channel添加到任务队列
  eventLoopAddTask(channel, ADD);
}

EventLoop *EventLoop::eventLoopInit(const char *name) {
  return new EventLoop(name);
}
int EventLoop::eventLoopRun() {
  // 取出事件分发和检测模型
  Dispatcher *dispatcher = dispatcher;
  // 循环进行事件处理
  while (!isQuit) {
    dispatcher->dispatch(this, 2);
    // 子线程自己修改自己的fd或子线程阻塞在dispatch里，主线程添加修改子线程的fd的事件唤醒子线程然后调用任务处理函数
    eventLoopProcessTask();
  }
  return 0;
}
int EventLoop::eventActivate(int fd, int event) {
  if (fd < 0)
    return -1;
  // 取出Channel
  Channel *channel = mChannelMap->getChannel(fd);
  assert(channel->getFd() == fd);
  if (event & ReadEvent && channel->readCallback) {
    channel->readCallback(channel->arg);
  }
  if (event & WriteEvent && channel->writeCallback) {
    channel->writeCallback(channel->arg);
  }
  return 0;
}
int EventLoop::eventLoopAddTask(Channel *channel, int type) {
  // 枷锁，保护共享资源
  {
    std::lock_guard<std::mutex> lock(mutex);
    // 创建新节点
    ChannelElement *node = new ChannelElement();
    node->channel = channel;
    node->type = type;
    mQueue.push(node);
  }
  // 处理节点
  /*
   *细节：
   *   1.对于节点的添加：可能是当前线程也可能是主线程
   *       (1).修改fd的事件，当前子线程发起，当前子线程处理
   *       (2).添加新的fd，添加任务的操作是由主线程发起的
   *   2.不能让主线程处理任务队列，需要由当前的子线程去处理
   */
  if (threadId == pthread_self()) {
    // 子线程
    eventLoopProcessTask();

  } else {
    // 主线程 --告诉子线程处理任务队列中的任务
    // 1.子线程工作 2.子线程被阻塞了：select poll epoll
    taskWakeUp(); // 往mair【0】写东西，mair[1]就会被激活
  }

  return 0;
}
int EventLoop::getThreadId() { return threadId; }
