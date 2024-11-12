#include "SelectDispatcher.h"
#include "Channel.h"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <unistd.h>

SelectDispatcher::SelectDispatcher() {
  FD_ZERO(&readSet);
  FD_ZERO(&writeSet);
  FD_ZERO(&exceptSet);
}
SelectDispatcher *SelectDispatcher::init() { return new SelectDispatcher(); }
int SelectDispatcher::setFdSet(Channel *channel) {
  if (channel->getEvents() & ReadEvent) {
    FD_SET(channel->getFd(), &readSet);
  }
  if (channel->getEvents() & WriteEvent) {
    FD_SET(channel->getFd(), &writeSet);
  }
  return 0;
}
int SelectDispatcher::clearFdSet(Channel *channel) {
  if (channel->getEvents() & ReadEvent) {
    FD_CLR(channel->getFd(), &readSet);
  }
  if (channel->getEvents() & WriteEvent) {
    FD_CLR(channel->getFd(), &writeSet);
  }
  return 0;
}

int SelectDispatcher::add(Channel *channel, EventLoop *evLoop) {
  if (channel->getFd() >= Max)
    return -1;
  setFdSet(channel);
  return 0;
}
int SelectDispatcher::modify(Channel *channel, EventLoop *evLoop) {

  setFdSet(channel);
  clearFdSet(channel);
  return 0;
}
int SelectDispatcher::remove(Channel *channel, EventLoop *evLoop) {
  clearFdSet(channel);
  return 0;
}
void SelectDispatcher::dispatch(EventLoop *evLoop, int timeout) {

  struct timeval val;
  val.tv_sec = timeout;
  val.tv_usec = 0;
  // 备份数据
  fd_set rdtmp = readSet;
  fd_set wrtmp = writeSet;
  int count = select(Max, &rdtmp, &wrtmp, nullptr, &val);
  if (count == -1) {
    perror("select");
    exit(0);
  }
  for (int i = 0; i < Max; i++) {
    if (FD_ISSET(i, &rdtmp)) {
      evLoop->eventActivate(i, ReadEvent);
    }
    if (FD_ISSET(i, &wrtmp)) {
      evLoop->eventActivate(i, WriteEvent);
    }
  }
}
int SelectDispatcher::clear(EventLoop *evLoop) { return 0; }
