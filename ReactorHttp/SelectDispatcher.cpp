#include "SelectDispatcher.h"
#include "Channel.h"
#include "Dispatcher.h"
#include "TcpConnection.h"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <unistd.h>

SelectDispatcher::SelectDispatcher(EventLoop *evLoop) : Dispatcher(evLoop) {
  FD_ZERO(&m_readSet);
  FD_ZERO(&m_writeSet);
  FD_ZERO(&m_exceptSet);
  m_name = "Select";
}
int SelectDispatcher::setFdSet() {
  if (m_channel->getEvents() & (int)FDEvent::ReadEvent) {
    FD_SET(m_channel->getFd(), &m_readSet);
  }
  if (m_channel->getEvents() & (int)FDEvent::WriteEvent) {
    FD_SET(m_channel->getFd(), &m_writeSet);
  }
  return 0;
}
int SelectDispatcher::clearFdSet() {
  if (m_channel->getEvents() & (int)FDEvent::ReadEvent) {
    FD_CLR(m_channel->getFd(), &m_readSet);
  }
  if (m_channel->getEvents() & (int)FDEvent::WriteEvent) {
    FD_CLR(m_channel->getFd(), &m_writeSet);
  }
  return 0;
}

int SelectDispatcher::add() {
  if (m_channel->getFd() >= m_maxSize)
    return -1;
  setFdSet();
  return 0;
}
int SelectDispatcher::modify() {
  if (m_channel->getEvents() & (int)FDEvent::ReadEvent) {
    FD_SET(m_channel->getFd(), &m_readSet);
    FD_CLR(m_channel->getFd(), &m_writeSet);
  }
  if (m_channel->getEvents() & (int)FDEvent::WriteEvent) {
    FD_SET(m_channel->getFd(), &m_writeSet);
    FD_CLR(m_channel->getFd(), &m_readSet);
  }
  return 0;
}
int SelectDispatcher::remove() {
  clearFdSet();
  m_channel->destroyCallback(
      const_cast<void *>(m_channel->getArg())); // arg为TcpConnection*
  return 0;
}
void SelectDispatcher::dispatch(int timeout) {

  struct timeval val;
  val.tv_sec = timeout;
  val.tv_usec = 0;
  // 备份数据
  fd_set rdtmp = m_readSet;
  fd_set wrtmp = m_writeSet;
  int count = select(m_maxSize, &rdtmp, &wrtmp, nullptr, &val);
  if (count == -1) {
    perror("select");
    exit(0);
  }
  for (int i = 0; i < m_maxSize; i++) {
    if (FD_ISSET(i, &rdtmp)) {
      m_evLoop->eventActivate(i, (int)FDEvent::ReadEvent);
    }
    if (FD_ISSET(i, &wrtmp)) {
      m_evLoop->eventActivate(i, (int)FDEvent::WriteEvent);
    }
  }
}
SelectDispatcher::~SelectDispatcher() {}
