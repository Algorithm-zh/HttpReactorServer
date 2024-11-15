#include "EpollDispatcher.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include <cstdio>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

EpollDispatcher::EpollDispatcher() {
  epfd = epoll_create(10);
  if (epfd == -1) {
    perror("epoll_create");
    exit(0);
  }
  events = (struct epoll_event *)calloc(Max, sizeof(struct epoll_event));
}
EpollDispatcher *EpollDispatcher::init() { return new EpollDispatcher(); }

int EpollDispatcher::epollCtl(Channel *channel, EventLoop *evLoop, int op) {
  struct epoll_event ev;
  ev.data.fd = channel->getFd();
  int events = 0;
  // 判断channel里的Events是什么事件
  if (channel->getEvents() & ReadEvent) {
    events |= EPOLLIN;
  }
  if (channel->getEvents() & WriteEvent) {
    events |= EPOLLOUT;
  }
  ev.events = events;
  int ret = epoll_ctl(epfd, op, channel->getFd(), &ev);
  return ret;
}
int EpollDispatcher::add(Channel *channel, EventLoop *evLoop) {
  int ret = epollCtl(channel, evLoop, EPOLL_CTL_ADD);
  if (ret == -1) {
    perror("epoll_ctl add");
    exit(0);
  }
  return ret;
}
int EpollDispatcher::modify(Channel *channel, EventLoop *evLoop) {
  int ret = epollCtl(channel, evLoop, EPOLL_CTL_MOD);
  if (ret == -1) {
    perror("epoll_ctl add");
    exit(0);
  }
  return ret;
}
int EpollDispatcher::remove(Channel *channel, EventLoop *evLoop) {
  int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
  if (ret == -1) {
    perror("epoll_ctl add");
    exit(0);
  }
  channel->destroyCallback(channel->arg); // arg为TcpConnection*
  delete (TcpConnection *)channel->arg;
  return ret;
}
void EpollDispatcher::dispatch(EventLoop *evLoop, int timeout) {

  int count = epoll_wait(epfd, events, Max, timeout * 1000);
  for (int i = 0; i < count; i++) {
    int event = events->events;
    int fd = events->data.fd;
    // 对端断开连接or对端断开连接仍在通信
    if (event & EPOLLERR || event & EPOLLHUP) {
      // 删除fd
    }
    if (event & EPOLLIN) {
      evLoop->eventActivate(fd, ReadEvent);
    }
    if (event & EPOLLOUT) {
      evLoop->eventActivate(fd, WriteEvent);
    }
  }
}
int EpollDispatcher::clear(EventLoop *evLoop) {
  free(events);
  close(epfd);
  return 0;
}
EpollDispatcher::~EpollDispatcher() {}
