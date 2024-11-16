#include "PollDispatcher.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <unistd.h>

PollDispatcher::PollDispatcher(EventLoop *evLoop) : Dispatcher(evLoop) {
  m_maxfd = 0;
  m_fds = new struct pollfd[m_maxNode];
  for (int i = 0; i < 1024; i++) {
    m_fds[i].fd = -1;
    m_fds[i].events = 0;
    m_fds[i].revents = 0;
  }
  m_name = "poll";
}

int PollDispatcher::add() {
  int events = 0;
  // 判断m_channel里的Events是什么事件
  if (m_channel->getEvents() & (int)FDEvent::ReadEvent) {
    events |= POLLIN;
  }
  if (m_channel->getEvents() & (int)FDEvent::WriteEvent) {
    events |= POLLOUT;
  }
  int i;
  for (i = 0; i < m_maxNode; i++) {
    if (m_fds[i].fd == -1) {
      m_fds[i].fd = m_channel->getFd();
      m_fds[i].events = events;
      m_maxfd = m_maxfd > i ? m_maxfd : i;
      break;
    }
  }
  if (i >= m_maxNode)
    return -1;
  return 0;
}
int PollDispatcher::modify() {
  int events = 0;
  // 判断m_channel里的Events是什么事件
  if (m_channel->getEvents() & (int)FDEvent::ReadEvent) {
    events |= POLLIN;
  }
  if (m_channel->getEvents() & (int)FDEvent::WriteEvent) {
    events |= POLLOUT;
  }
  int i;
  for (i = 0; i < m_maxNode; i++) {
    if (m_fds[i].fd == m_channel->getFd()) {
      m_fds[i].events = events;
      break;
    }
  }
  if (i >= m_maxNode)
    return -1;
  return 0;
}
int PollDispatcher::remove() {
  int events = 0;
  int i;
  for (i = 0; i < m_maxNode; i++) {
    if (m_fds[i].fd == m_channel->getFd()) {
      m_fds[i].fd = -1;
      m_fds[i].events = 0;
      m_fds[i].revents = 0;
      break;
    }
  }
  // 通过m_channel释放对应的TcpConnection资源
  m_channel->destroyCallback(const_cast<void *>(m_channel->getArg()));

  if (i >= m_maxNode)
    return -1;
  return 0;
}
void PollDispatcher::dispatch(int timeout) {

  int count = poll(m_fds, m_maxfd + 1, timeout * 1000);
  if (count == -1) {
    perror("poll");
    exit(0);
  }
  for (int i = 0; i <= m_maxfd; i++) {
    if (m_fds[i].fd == -1)
      continue;
    // 对端断开连接or对端断开连接仍在通信
    if (m_fds[i].revents & EPOLLIN) {
      m_evLoop->eventActivate(m_fds[i].fd, (int)FDEvent::ReadEvent);
    }
    if (m_fds[i].revents & EPOLLOUT) {
      m_evLoop->eventActivate(m_fds[i].fd, (int)FDEvent::WriteEvent);
    }
  }
}
PollDispatcher::~PollDispatcher() { delete[] m_fds; }
