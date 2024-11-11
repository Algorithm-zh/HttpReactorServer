#pragma once
#include "Dispatcher.h"
#include <sys/epoll.h>
#define Max 520
class EpollDispatcher : public Dispatcher {
public:
  EpollDispatcher();
  virtual ~EpollDispatcher();

  static EpollDispatcher *init();

  virtual int add(Channel *channel, EventLoop *evLoop);
  virtual int modify(Channel *channel, EventLoop *evLoop);
  virtual int remove(Channel *channel, EventLoop *evLoop);
  virtual void dispatch(EventLoop *evLoop, int timeout);
  virtual int clear(EventLoop *evLoop);

  int epollCtl(Channel *channel, EventLoop *evLoop, int op);

private:
  int epfd;
  struct epoll_event *events;
};
