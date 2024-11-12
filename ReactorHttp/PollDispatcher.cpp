#include "PollDispatcher.h"
#include "Channel.h"
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <unistd.h>

PollDispatcher::PollDispatcher() {
  maxfd = 0;
  for (int i = 0; i < Max; i++) {
    fds[i].fd = -1;
    fds[i].events = 0;
    fds[i].revents = 0;
  }
}
PollDispatcher *PollDispatcher::init() { return new PollDispatcher(); }

int PollDispatcher::add(Channel *channel, EventLoop *evLoop) {
  int events = 0;
  // 判断channel里的Events是什么事件
  if (channel->getEvents() & ReadEvent) {
    events |= POLLIN;
  }
  if (channel->getEvents() & WriteEvent) {
    events |= POLLOUT;
  }
  int i;
  for (i = 0; i < Max; i++) {
    if (fds[i].fd == -1) {
      fds[i].fd = channel->getFd();
      fds[i].events = events;
      maxfd = maxfd > i ? maxfd : i;
      break;
    }
  }
  if (i >= Max)
    return -1;
  return 0;
}
int PollDispatcher::modify(Channel *channel, EventLoop *evLoop) {
  int events = 0;
  // 判断channel里的Events是什么事件
  if (channel->getEvents() & ReadEvent) {
    events |= POLLIN;
  }
  if (channel->getEvents() & WriteEvent) {
    events |= POLLOUT;
  }
  int i;
  for (i = 0; i < Max; i++) {
    if (fds[i].fd == channel->getFd()) {
      fds[i].events = events;
      break;
    }
  }
  if (i >= Max)
    return -1;
  return 0;
}
int PollDispatcher::remove(Channel *channel, EventLoop *evLoop) {
  int events = 0;
  int i;
  for (i = 0; i < Max; i++) {
    if (fds[i].fd == channel->getFd()) {
      fds[i].fd = -1;
      fds[i].events = 0;
      fds[i].revents = 0;
      break;
    }
  }
  if (i >= Max)
    return -1;
  return 0;
}
void PollDispatcher::dispatch(EventLoop *evLoop, int timeout) {

  int count = poll(fds, maxfd + 1, timeout * 1000);
  if (count == -1) {
    perror("poll");
    exit(0);
  }
  for (int i = 0; i <= maxfd; i++) {
    if (fds[i].fd == -1)
      continue;
    // 对端断开连接or对端断开连接仍在通信
    if (fds[i].revents & EPOLLIN) {
      evLoop->eventActivate(fds[i].fd, ReadEvent);
    }
    if (fds[i].revents & EPOLLOUT) {
      evLoop->eventActivate(fds[i].fd, WriteEvent);
    }
  }
}
int PollDispatcher::clear(EventLoop *evLoop) { return 0; }
