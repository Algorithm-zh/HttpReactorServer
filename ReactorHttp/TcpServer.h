#pragma once
#include "EventLoop.h"
#include "ThreadPool.h"
class Listner {

public:
  Listner(unsigned short port);
  static Listner *listnerInit(unsigned short port);
  int getFd() { return lfd; };

private:
  int lfd;
  unsigned short port;
};
class TcpServer {
public:
  TcpServer(unsigned short port, int threadNum);
  // 初始化
  static TcpServer *TcpServerInit(unsigned short port, int threadNum);
  // 启动服务器
  void tcpServerRun();
  // 建立连接
  static int acceptConnection(void *arg);

private:
  EventLoop *mainLoop;
  ThreadPool *threadPool;
  Listner *listner;
  int threadNum;
};
