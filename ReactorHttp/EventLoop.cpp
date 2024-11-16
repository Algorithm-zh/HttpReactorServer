#include "EventLoop.h"
#include "Channel.h"
#include "EpollDispatcher.h"
#include "Log.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <mutex>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>

EventLoop::EventLoop(const std::string threadName) {
  m_isQuit = true;
  m_dispatcher = new EpollDispatcher(this);
  m_threadId = std::this_thread::get_id();
  m_threadName = threadName == std::string() ? "MainThread" : threadName;
  m_channelMap.clear();
  // 两个fd之间本地网络通信
  int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, m_socketpair);
  if (ret == -1) {
    perror("socketpair");
    exit(0);
  }
  // 指定规则msocketpair[0] 发送数据，msocketpair[1]接收数据
  // 绑定, functional不能直接对类的成员函数进行打包，所以需要先用绑定器绑定
  auto obj = std::bind(&EventLoop::readLocalMessage, this);
  Channel *channel = new Channel(m_socketpair[1], FDEvent::ReadEvent, obj,
                                 nullptr, nullptr, this);
  // channel添加到任务队列
  addTask(channel, ElemType::ADD);
}

EventLoop::EventLoop() : EventLoop(std::string()) {
  // 委托构造函数,直接调用代餐构造函数里的内容
}

EventLoop::~EventLoop() {}

// 写数据
// 目的就是往pair[0]写数据以此激活pair[1]
void EventLoop::taskWakeUp() {
  Debug("激活子线程");
  const char *msg = "激活激活";
  write(m_socketpair[0], msg, strlen(msg));
}

// 读数据
int EventLoop::readLocalMessage() {
  char buf[256];
  read(m_socketpair[1], buf, sizeof(buf));
  return 0;
}

int EventLoop::run() {
  m_isQuit = false;
  // 比较线程id是否正常
  if (m_threadId != std::this_thread::get_id())
    return -1;
  // 循环进行事件处理
  while (!m_isQuit) {
    m_dispatcher->dispatch();
    // 子线程自己修改自己的fd或子线程阻塞在dispatch里，主线程添加修改子线程的fd的事件唤醒子线程然后调用任务处理函数
    processTaskQ();
  }
  return 0;
}

int EventLoop::eventActivate(int fd, int event) {
  if (fd < 0)
    return -1;
  // 取出Channel
  Channel *channel = m_channelMap[fd];
  assert(channel->getFd() == fd);
  if (event & (int)FDEvent::ReadEvent && channel->readCallback) {
    channel->readCallback(const_cast<void *>(channel->getArg()));
  }
  if (event & (int)FDEvent::WriteEvent && channel->writeCallback) {
    channel->writeCallback(const_cast<void *>(channel->getArg()));
  }
  return 0;
}

int EventLoop::addTask(Channel *channel, ElemType type) {

  // 枷锁，保护共享资源
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    // 创建新节点
    ChannelElement *node = new ChannelElement();
    node->channel = channel;
    node->type = type;
    m_taskQ.push(node);
  }
  // 处理节点
  /*
   *细节：
   *   1.对于节点的添加：可能是当前线程也可能是主线程
   *       (1).修改fd的事件，当前子线程发起，当前子线程处理
   *       (2).添加新的fd，添加任务的操作是由主线程发起的
   *   2.不能让主线程处理任务队列，需要由当前的子线程去处理
   */
  if (m_threadId == std::this_thread::get_id()) {
    // 子线程
    processTaskQ();

  } else {
    // 主线程 --告诉子线程处理任务队列中的任务
    // 1.子线程工作 2.子线程被阻塞了：select poll epoll
    taskWakeUp(); // 往mair【0】写东西，mair[1]就会被激活
  }

  return 0;
}

int EventLoop::processTaskQ() {
  Debug("ChannelElementQueue size:%ld", m_taskQ.size());
  while (!m_taskQ.empty()) {
    m_mutex.lock();
    ChannelElement *node = m_taskQ.front();
    m_mutex.unlock();
    m_taskQ.pop();
    Channel *channel = node->channel;
    if (node->type == ElemType::ADD) {
      add(channel);
    } else if (node->type == ElemType::MODIFY) {
      modify(channel);
    } else if (node->type == ElemType::DELETE) {
      remove(channel);
    }
    Debug("释放ChannelElement before");
    delete node;
    Debug("释放ChannelElement after");
  }
  return 0;
}

int EventLoop::add(Channel *channel) {
  int fd = channel->getFd();
  if (m_channelMap.find(fd) == m_channelMap.end()) {
    m_channelMap.insert(std::make_pair(fd, channel));
    m_dispatcher->setChannel(channel);
    int ret = m_dispatcher->add();
    return ret;
  }
  return -1;
}

int EventLoop::remove(Channel *channel) {
  int fd = channel->getFd();
  m_dispatcher->setChannel(channel);
  int ret = m_dispatcher->remove();
  return ret;
}

int EventLoop::modify(Channel *channel) {
  int fd = channel->getFd();
  if (m_channelMap.find(fd) == m_channelMap.end())
    return -1;
  int ret = m_dispatcher->modify();
  return ret;
}

int EventLoop::freeChannel(Channel *channel) {
  // 删除channel和fd的对应关系
  auto it = m_channelMap.find(channel->getFd());
  if (it != m_channelMap.end()) {
    m_channelMap.erase(it);
    close(channel->getFd());
    delete channel;
  }
  return 0;
}
