#pragma once
#include "Channel.h"
#include "EventLoop.h"

class EventLoop;
// EventLoop包含Dispatcher和DispatcherData
class Dispatcher {
public:
  virtual ~Dispatcher();
  virtual int add(Channel *channel, EventLoop *evLoop) = 0;
  virtual int modify(Channel *channel, EventLoop *evLoop) = 0;
  virtual int remove(Channel *channel, EventLoop *evLoop) = 0;
  virtual void dispatch(EventLoop *evLoop, int timeout) = 0;
  virtual int clear(EventLoop *evLoop) = 0;

protected:
  Dispatcher();
};
