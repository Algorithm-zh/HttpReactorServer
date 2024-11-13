#pragma once
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
class TcpConnection {
public:
  TcpConnection(int fd, EventLoop *evLoop);
  // TcpConnection的EventLoop来自子线程
  static TcpConnection *tcpConnectionInit(int fd, EventLoop *evLoop);
  static int processRead(void *arg);

private:
  EventLoop *evLoop;
  Channel *channel;
  Buffer *readBuf;
  Buffer *writeBuf;
  char name[32];
};
