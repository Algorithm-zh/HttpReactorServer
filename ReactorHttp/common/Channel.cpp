#include "Channel.h"
Channel::Channel(int fd, FDEvent events, handleFunc readCallback,
                 handleFunc writeCallback, handleFunc destroyCallback,
                 void *arg)
    : m_fd(fd), m_events((int)events), readCallback(readCallback),
      writeCallback(writeCallback), destroyCallback(destroyCallback),
      m_arg(arg) {}

void Channel::writeEventEnable(bool flag) {
  if (flag)
    m_events |= static_cast<int>(FDEvent::WriteEvent); // 检测写事件
  // c++类型转换
  else
    m_events =
        m_events & ~static_cast<int>(FDEvent::WriteEvent); // 不检测写事件
}
bool Channel::isWriteEventEnable() {
  return m_events & static_cast<int>(FDEvent::WriteEvent);
}
