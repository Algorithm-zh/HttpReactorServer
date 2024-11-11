#include "Channel.h"
Channel *Channel::channelInit(int fd, int events, handleFunc readCallback,
                              handleFunc writeCallback, void *arg) {
  return new Channel(fd, events, readCallback, writeCallback, arg);
}
Channel::Channel(int fd, int events, handleFunc readCallback,
                 handleFunc writeCallback, void *arg)
    : fd(fd), events(events), readCallback(readCallback),
      writeCallback(writeCallback), arg(arg) {}

void Channel::writeEventEnable(Channel *channel, bool flag) {
  if (flag)
    channel->events |= WriteEvent; // 检测写事件
  else
    channel->events = channel->events & ~WriteEvent; // 设置不检测写事件
}
bool Channel::isWriteEventEnable(Channel *channel) {
  return channel->events & WriteEvent;
}
int Channel::getFd() { return fd; }

int Channel::getEvents() { return events; };
