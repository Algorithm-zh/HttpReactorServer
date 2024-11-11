#pragma once
#include "Dispatcher.h"
#include <sys/select.h>
#define Max 1024
class SelectDispatcher : public Dispatcher {
public:
  SelectDispatcher();
  virtual ~SelectDispatcher();

  static SelectDispatcher *init();

  virtual int add(Channel *channel, EventLoop *evLoop);
  virtual int modify(Channel *channel, EventLoop *evLoop);
  virtual int remove(Channel *channel, EventLoop *evLoop);
  virtual void dispatch(EventLoop *evLoop, int timeout);
  virtual int clear(EventLoop *evLoop);

  int setFdSet(Channel *channel);
  int clearFdSet(Channel *channel);

private:
  fd_set readSet;
  fd_set writeSet;
  fd_set exceptSet;
};
