#include "TcpServer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include <arpa/inet.h>
#include <sys/socket.h>

TcpServer::TcpServer(unsigned short port, int threadNum) {
  listner = Listner::listnerInit(port);
  mainLoop = EventLoop::eventLoopInit("MainLoop");
  this->threadNum = threadNum;
  threadPool = ThreadPool::threadPoolInit(mainLoop, threadNum);
}
// 初始化
TcpServer *TcpServer::TcpServerInit(unsigned short port, int threadNum) {
  return new TcpServer(port, threadNum);
}
int TcpServer::acceptConnection(void *arg) {
  TcpServer *server = (TcpServer *)arg;
  // 和客户端建立连接
  int cfd = accept(server->listner->getFd(), nullptr, nullptr);
  // 从线程池中取出一个子线程的反应堆实例去处理事件
  EventLoop *evLoop = server->threadPool->takeWorkerEventLoop();
  // 将cfd放到TcpConnection处理
  TcpConnection::tcpConnectionInit(cfd, evLoop);

  return 0;
}
void TcpServer::tcpServerRun() {
  // 启动线程池
  threadPool->threadPoolRun();
  // 添加检测的任务
  // 初始化一个channel
  Channel *channel = Channel::channelInit(listner->getFd(), ReadEvent,
                                          acceptConnection, nullptr, this);
  mainLoop->eventLoopAddTask(channel, ADD);
  // 启动反应堆模型
  mainLoop->eventLoopRun();
}

// 初始化监听
Listner *Listner::listnerInit(unsigned short port) { return new Listner(port); }
Listner::Listner(unsigned short port) {
  // 1.创建监听的fd
  int lfd = socket(AF_INET, SOCK_STREAM, 0); // 0为tcp
  if (lfd == -1) {
    perror("socket");
    return;
  }
  // 2.设置端口复用
  int opt = 1;
  int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (ret == -1) {
    perror("setsockopt");
    return;
  }
  // 3.绑定
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
  ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind");
    return;
  }
  // 4.监听
  ret = listen(lfd, 128);
  if (ret == -1) {
    perror("listen");
    return;
  }
  this->lfd = lfd;
  this->port = port;
}
