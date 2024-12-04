#pragma once
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
// #define MSG_SEND_AUTO
class TcpConnection {
public:
  // TcpConnection的EventLoop来自子线程
  TcpConnection(int fd, EventLoop *evLoop);
  ~TcpConnection();
  static int processRead(void *arg);
  static int processWrite(void *arg);
  static int destroy(void *arg);

private:
  EventLoop *m_evLoop;
  Channel *m_channel;
  Buffer *m_readBuf;
  Buffer *m_writeBuf;
  std::string m_name;
  HttpRequest *m_request;
  HttpResponse *m_response;
};
