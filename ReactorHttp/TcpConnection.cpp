#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <cstdio>

TcpConnection::TcpConnection(int fd, EventLoop *evLoop) {
  this->evLoop = evLoop;
  readBuf = Buffer::bufferInit(10240);
  writeBuf = Buffer::bufferInit(10240);
  request = HttpRequest::httpRequestInit();
  response = HttpResponse::httpResponseInit();
  sprintf(name, "Connection-%d", fd);
  channel = Channel::channelInit(fd, ReadEvent, processRead, processWrite,
                                 tcpConnectionDestroy, this);
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
    int socket = conn->channel->getFd();
#ifdef MSG_SEND_AUTO
    conn->channel->writeEventEnable(conn->channel, true);
    conn->evLoop->eventLoopAddTask(conn->channel, MODEIFY);
#endif
    bool flag = conn->request->parseHttpRequest(conn->readBuf, conn->response,
                                                conn->writeBuf, socket);
    if (!flag) {
      // 解析失败，回复一个简单的html
      char *errMsg = (char *)"Http/1.1 400 Bad Request\r\n\r\n";
      conn->writeBuf->bufferAppendString(errMsg);
    }
  }
  // 断开连接
#ifndef MSG_SEND_AUTO
  conn->evLoop->eventLoopAddTask(conn->channel, DELETE);
#endif
  return 0;
}
int TcpConnection::processWrite(void *arg) {

  TcpConnection *conn = (TcpConnection *)arg;
  // 发送数据
  int count = conn->writeBuf->bufferSendData(conn->channel->getFd());
  if (count > 0) {
    // 判断是否有没全部被发送出了
    if (conn->writeBuf->bufferReadableSize() == 0) {
      // 1.不再检测写事件 -- 修改channel中保存的事件
      conn->channel->writeEventEnable(conn->channel, false);
      // 2.修改dispatcher检测的集合 -- 添加任务节点
      conn->evLoop->eventLoopAddTask(conn->channel, MODEIFY);
      // 3.删除这个节点
      conn->evLoop->eventLoopAddTask(conn->channel, DELETE);
    }
  }
  return count;
}

int TcpConnection::tcpConnectionDestroy(void *arg) {

  TcpConnection *conn = (TcpConnection *)arg;
  if (conn->readBuf && conn->readBuf->bufferReadableSize() == 0 &&
      conn->writeBuf && conn->writeBuf->bufferWriteableSize() == 0) {
    conn->evLoop->destoryChannel(conn->channel);
    conn->readBuf->bufferDestory();
    conn->writeBuf->bufferDestory();
    conn->request->httpRequestDestroy();
    conn->response->httpResonseDestroy();
  }
  return 0;
}
