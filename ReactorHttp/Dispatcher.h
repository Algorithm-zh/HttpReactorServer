#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include <string>

class EventLoop;
// EventLoop包含Dispatcher和DispatcherData
class Dispatcher {
public:
  // 使用多态时需要虚函数析构函数
  Dispatcher(EventLoop *evLoop);
  virtual ~Dispatcher();
  virtual int add() = 0;
  virtual int modify() = 0;
  virtual int remove() = 0;
  virtual void dispatch(int timeout = 2) = 0;
  inline void setChannel(Channel *channel) { m_channel = channel; }

protected:
  std::string m_name = std::string();
  Channel *m_channel;
  EventLoop *m_evLoop;
};
