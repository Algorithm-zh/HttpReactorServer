#pragma once
#include "EventLoop.h"
#include "ThreadPool.h"

class TcpServer {
public:
  TcpServer(unsigned short port, int threadNum);
  ~TcpServer();
  // 初始化监听
  void setListen();
  // 启动服务器
  void run();
  // 建立连接
  int acceptConnection();

  int getFd() { return m_lfd; };

private:
  EventLoop *m_mainLoop;
  ThreadPool *m_threadPool;
  int m_threadNum;
  int m_lfd;
  unsigned short m_port;
};
