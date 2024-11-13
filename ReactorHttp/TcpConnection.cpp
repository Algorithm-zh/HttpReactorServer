#include "TcpConnection.h"
#include "Channel.h"
#include <cstdio>

TcpConnection::TcpConnection(int fd, EventLoop *evLoop) {
  this->evLoop = evLoop;
  readBuf = Buffer::bufferInit(10240);
  writeBuf = Buffer::bufferInit(10240);
  sprintf(name, "Connection-%d", fd);
  channel = Channel::channelInit(fd, ReadEvent, processRead, nullptr, nullptr);
  evLoop->eventLoopAddTask(channel, ADD);
}
// TcpConnection的EventLoop来自子线程
TcpConnection *TcpConnection::tcpConnectionInit(int fd, EventLoop *evLoop) {
  return new TcpConnection(fd, evLoop);
}
int TcpConnection::processRead(void *arg) {
  TcpConnection *conn = (TcpConnection *)arg;
  // 接收数据
  int count = conn->readBuf->bufferSocketRead(conn->channel->getFd());
  if (count > 0) {
    // 接收到了http请求，解析http请求
  } else {
    // 断开连接
  }
  return 0;
}
