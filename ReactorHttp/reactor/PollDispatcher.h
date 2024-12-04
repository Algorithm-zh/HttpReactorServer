#pragma once
#include "Dispatcher.h"
#include <sys/poll.h>
class PollDispatcher : public Dispatcher {
public:
  PollDispatcher(EventLoop *evLoop);
  ~PollDispatcher();

  static PollDispatcher *init();

  int add() override;
  int modify() override;
  int remove() override;
  void dispatch(int timeout = 2) override;

private:
  int m_maxfd;
  struct pollfd *m_fds;
  const int m_maxNode = 1024;
};
