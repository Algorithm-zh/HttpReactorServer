#pragma once
#include "Dispatcher.h"
#include <sys/select.h>
class SelectDispatcher : public Dispatcher {
public:
  SelectDispatcher(EventLoop *evLoop);
  ~SelectDispatcher();

  int add() override;
  int modify() override;
  int remove() override;
  void dispatch(int timeout = 2) override;

private:
  fd_set m_readSet;
  fd_set m_writeSet;
  fd_set m_exceptSet;
  const int m_maxSize = 1024;

  int setFdSet();
  int clearFdSet();
};
