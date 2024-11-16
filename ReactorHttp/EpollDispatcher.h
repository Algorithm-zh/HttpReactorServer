#pragma once
#include "Dispatcher.h"
#include <sys/epoll.h>
class EpollDispatcher : public Dispatcher {
public:
  EpollDispatcher(EventLoop *evLoop);
  ~EpollDispatcher();

  static EpollDispatcher *init();

  int add() override;
  int modify() override;
  int remove() override;
  void dispatch(int timeout = 2) override;

private:
  int m_epfd;
  struct epoll_event *m_events;
  const int m_maxNode = 520;

  int epollCtl(int op);
};
