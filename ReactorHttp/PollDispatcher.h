#pragma once
#include "Dispatcher.h"
#include <sys/poll.h>
#define Max 1024
class PollDispatcher : public Dispatcher {
public:
  PollDispatcher();
  virtual ~PollDispatcher();

  static PollDispatcher *init();

  virtual int add(Channel *channel, EventLoop *evLoop);
  virtual int modify(Channel *channel, EventLoop *evLoop);
  virtual int remove(Channel *channel, EventLoop *evLoop);
  virtual void dispatch(EventLoop *evLoop, int timeout);
  virtual int clear(EventLoop *evLoop);

private:
  int maxfd;
  struct pollfd fds[Max];
};
