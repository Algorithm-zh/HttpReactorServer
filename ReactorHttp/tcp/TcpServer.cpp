#include "TcpServer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Log.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include <arpa/inet.h>
#include <functional>
#include <sys/socket.h>

TcpServer::TcpServer(unsigned short port, int threadNum) {
  m_mainLoop = new EventLoop();
  m_port = port;
  m_threadNum = threadNum;
  m_threadPool = new ThreadPool(m_mainLoop, threadNum);
  setListen();
}

TcpServer::~TcpServer() {
  delete m_mainLoop;
  delete m_threadPool;
}

int TcpServer::acceptConnection() {
  // 和客户端建立连接
  int cfd = accept(m_lfd, NULL, NULL);
  // 从线程池中取出一个子线程的反应堆实例去处理事件
  EventLoop *evLoop = m_threadPool->takeWorkerEventLoop();
  // 将cfd放到TcpConnection处理
  new TcpConnection(cfd, evLoop);
  return 0;
}

void TcpServer::run() {
  // 启动线程池
  m_threadPool->run();
  // 添加检测的任务
  // 初始化一个channel
  // 绑定
  auto obj = std::bind(&TcpServer::acceptConnection, this);
  Channel *channel =
      new Channel(m_lfd, FDEvent::ReadEvent, obj, nullptr, nullptr, this);

  m_mainLoop->addTask(channel, ElemType::ADD);
  // 启动反应堆模型
  m_mainLoop->run();
  Debug("服务器程序启动");
}

// 初始化监听
void TcpServer::setListen() {
  // 1.创建监听的fd
  m_lfd = socket(AF_INET, SOCK_STREAM, 0); // 0为tcp
  if (m_lfd == -1) {
    perror("socket");
    return;
  }
  // 2.设置端口复用
  int opt = 1;
  int ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (ret == -1) {
    perror("setsockopt");
    return;
  }
  // 3.绑定
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(m_port);
  ret = bind(m_lfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind");
    return;
  }
  // 4.监听
  ret = listen(m_lfd, 128);
  if (ret == -1) {
    perror("listen");
    return;
  }
}
