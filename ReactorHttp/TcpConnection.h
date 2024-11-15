#pragma once
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#define MSG_SEND_AUTO
class TcpConnection {
public:
  TcpConnection(int fd, EventLoop *evLoop);
  // TcpConnection的EventLoop来自子线程
  static TcpConnection *tcpConnectionInit(int fd, EventLoop *evLoop);
  static int processRead(void *arg);
  static int processWrite(void *arg);
  static int tcpConnectionDestroy(void *arg);

  HttpRequest *request;
  HttpResponse *response;

private:
  EventLoop *evLoop;
  Channel *channel;
  Buffer *readBuf;
  Buffer *writeBuf;
  char name[32];
};
