#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Log.h"
#include <cstdio>
#include <string>

TcpConnection::TcpConnection(int fd, EventLoop *evLoop) {
  m_evLoop = evLoop;
  m_readBuf = new Buffer(10240);
  m_writeBuf = new Buffer(10240);
  m_request = new HttpRequest();
  m_response = new HttpResponse();
  m_name = "Connection-" + std::to_string(fd);
  m_channel = new Channel(fd, FDEvent::ReadEvent, processRead, processWrite,
                          destroy, this);
  evLoop->addTask(m_channel, ElemType::ADD);
}

TcpConnection::~TcpConnection() {
  if (m_readBuf && m_readBuf->readableSize() == 0 && m_writeBuf &&
      m_writeBuf->writeableSize() == 0) {
    delete m_readBuf;
    delete m_writeBuf;
    delete m_request;
    delete m_response;
    m_evLoop->freeChannel(m_channel);
  }
  Debug("连接断开释放资源，connName:%s", m_name.data());
}

int TcpConnection::processRead(void *arg) {
  TcpConnection *conn = (TcpConnection *)arg;
  // 接收到了http请求，解析http请求
  int socket = conn->m_channel->getFd();
  int count = conn->m_readBuf->socketRead(socket);
  Debug("接收到http请求数据：%s", conn->m_readBuf->beginRead());
  if (count > 0) {
#ifdef MSG_SEND_AUTO
    conn->m_channel->writeEventEnable(true);
    conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
#endif
    bool flag = conn->m_request->parseHttpRequest(
        conn->m_readBuf, conn->m_response, conn->m_writeBuf, socket);
    Debug("解析消息完毕");
    if (!flag) {
      // 解析失败，回复一个简单的html
      std::string errMsg = (char *)"Http/1.1 400 Bad Request\r\n\r\n";
      conn->m_writeBuf->appendString(errMsg);
    }
  } else {
#ifdef MSG_SEND_AUTO
    conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
  }
  // 断开连接
#ifndef MSG_SEND_AUTO
  conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
  return 0;
}
int TcpConnection::processWrite(void *arg) {

  Debug("开始发送数据（基于写事件发送）");
  TcpConnection *conn = (TcpConnection *)arg;
  // 发送数据
  int count = conn->m_writeBuf->sendData(conn->m_channel->getFd());
  if (count > 0) {
    // 判断是否有没全部被发送出了
    if (conn->m_writeBuf->readableSize() == 0) {
      // 1.不再检测写事件 -- 修改channel中保存的事件
      conn->m_channel->writeEventEnable(false);
      // 2.修改dispatcher检测的集合 -- 添加任务节点
      conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
      // 3.删除这个节点
      conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
    }
  }
  return count;
}

int TcpConnection::destroy(void *arg) {

  TcpConnection *conn = (TcpConnection *)arg;
  if (conn != nullptr)
    delete conn;
  return 0;
}
