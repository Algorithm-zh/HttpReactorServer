#include "EpollDispatcher.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Log.h"
#include "TcpConnection.h"
#include <cstdio>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

EpollDispatcher::EpollDispatcher(EventLoop *evLoop) : Dispatcher(evLoop) {
  m_epfd = epoll_create(10);
  if (m_epfd == -1) {
    perror("epoll_create");
    exit(0);
  }
  m_name = "Epoll";
  m_events = new struct epoll_event[m_maxNode];
}

int EpollDispatcher::epollCtl(int op) {
  struct epoll_event ev;
  ev.data.fd = m_channel->getFd();
  int events = 0;
  // 判断channel里的Events是什么事件
  if (m_channel->getEvents() & (int)FDEvent::ReadEvent) {
    events |= EPOLLIN;
  }
  if (m_channel->getEvents() & (int)FDEvent::WriteEvent) {
    events |= EPOLLOUT;
  }
  Debug("开始检测事件: %d", events);
  ev.events = events;
  int ret = epoll_ctl(m_epfd, op, m_channel->getFd(), &ev);
  return ret;
}

int EpollDispatcher::add() {
  int ret = epollCtl(EPOLL_CTL_ADD);
  if (ret == -1) {
    perror("epoll_ctl add");
    exit(0);
  }
  return ret;
}

int EpollDispatcher::modify() {
  int ret = epollCtl(EPOLL_CTL_MOD);
  if (ret == -1) {
    perror("epoll_ctl add");
    exit(0);
  }
  return ret;
}

int EpollDispatcher::remove() {
  int ret = epollCtl(EPOLL_CTL_DEL);
  if (ret == -1) {
    perror("epoll_ctl add");
    exit(0);
  }
  m_channel->destroyCallback(
      const_cast<void *>(m_channel->getArg())); // arg为TcpConnection*
  return ret;
}

void EpollDispatcher::dispatch(int timeout) {

  int count = epoll_wait(m_epfd, m_events, m_maxNode, timeout * 1000);
  for (int i = 0; i < count; i++) {
    int event = m_events->events;
    int fd = m_events->data.fd;
    // 对端断开连接or对端断开连接仍在通信
    if (event & EPOLLERR || event & EPOLLHUP) {
      // 删除fd
    }
    if (event & EPOLLIN) {
      m_evLoop->eventActivate(fd, (int)FDEvent::ReadEvent);
    }
    if (event & EPOLLOUT) {
      m_evLoop->eventActivate(fd, (int)FDEvent::WriteEvent);
    }
  }
}

EpollDispatcher::~EpollDispatcher() {
  close(m_epfd);
  delete[] m_events;
}
