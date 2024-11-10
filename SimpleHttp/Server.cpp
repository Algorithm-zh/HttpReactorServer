#include "Server.h"
#include <arpa/inet.h>
#include <assert.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>

int initListenFd(unsigned short port) {
  // 1.创建监听的fd
  int lfd = socket(AF_INET, SOCK_STREAM, 0); // 0为tcp
  if (lfd == -1) {
    perror("socket");
    return -1;
  }
  // 2.设置端口复用
  int opt = 1;
  int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  if (ret == -1) {
    perror("setsockopt");
    return -1;
  }
  // 3.绑定
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);
  ret = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }
  // 4.监听
  ret = listen(lfd, 128);
  if (ret == -1) {
    perror("listen");
    return -1;
  }
  return lfd;
}

int epollRun(int lfd) {
  // 1.创建epoll实例
  int epfd = epoll_create(1);
  if (epfd == -1) {
    perror("epoll_create");
    return -1;
  }
  // 2. lfd上树
  struct epoll_event ev;
  ev.data.fd = lfd;
  ev.events = EPOLLIN;
  int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
  if (epfd == -1) {
    perror("epoll_ctl");
    return -1;
  }
  // 3. 检测
  struct epoll_event evs[1024];
  int size = sizeof(evs) / sizeof(struct epoll_event);
  while (1) {
    int num = epoll_wait(epfd, evs, size, -1);
    for (int i = 0; i < num; i++) {
      int fd = evs[i].data.fd;
      if (fd == lfd) {
        // 建立新链接，调用accept
        acceptClient(lfd, epfd);
      } else {
        // 主要是接收对端的数据
        recvHttpRequest(fd, epfd);
      }
    }
  }
}
int acceptClient(int lfd, int epfd) {
  // 1.建立连接
  int cfd = accept(lfd, NULL, NULL);
  if (cfd == -1) {
    perror("accept");
    return -1;
  }

  // 2.设置非阻塞
  int flag = fcntl(cfd, F_GETFL);
  flag |= O_NONBLOCK;
  fcntl(cfd, F_SETFL, flag);

  // 3. cfd 添加到epoll里
  struct epoll_event ev;
  ev.data.fd = cfd;
  ev.events = EPOLLIN | EPOLLET;
  int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
  if (epfd == -1) {
    perror("epoll_ctl");
    return -1;
  }

  return 0;
}
int recvHttpRequest(int cfd, int epfd) {
  int len = 0, total = 0;
  char tmp[1024] = {0}; // 防止buf数据被覆盖
  char buf[4096] = {0};
  while ((len = recv(cfd, buf, sizeof(tmp), 0)) > 0) {
    if (total + len < sizeof buf)
      memcpy(buf + total, tmp, len);
    total += len;
  }
  // 判读数据是否接收完毕
  if (len == -1 && errno == EAGAIN) {
    // 解析请求行

  } else if (len == 0) {
    // 客户端断开了连接
    epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
    close(cfd);
  } else {
    perror("recv");
  }
  return 0;
}
int parseRequestLine(const char *line, int cfd) {
  // 解析请求行
  char method[12];
  char path[1024];
  sscanf(line, "%[^ ] %[^ ]", method, path);
  if (strcasecmp(method, "get") != 0) {
    return -1; // 只解析get请求
  }

  // 处理客户端请求的静态资源
  char *file = nullptr;
  if (strcmp(path, "/") == 0) {
    file = (char *)"./";
  } else {
    file = path + 1;
  }
  // 获取文件属性
  struct stat st;
  int ret = stat(file, &st);
  if (ret == -1) {
    // 文件不存在
    sendHeadMsg(cfd, 404, "Not Found", getFileType((char *)".html"), -1);
    sendFile("404.html", cfd);
  }
  // 判断文件类型
  if (S_ISDIR(st.st_mode)) {
    // 目录， 将目录的内容发给客户端

  } else {
    // 文件， 将文件内容发给客户端
    sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
    sendFile(file, cfd);
  }

  return 0;
}
int sendFile(const char *fileName, int cfd) {

  // 1. 打开文件
  int fd = open(fileName, O_RDONLY);
  assert(fd > 0);

#if 0
  while(1)
  {
    char buf[1024];
    int len = read(fd, buf, sizeof buf);
    if(len > 0)
    {
      send(cfd, buf, len, 0);
      usleep(10);// 让数据发的慢点，以便于对端处理数据

    }else if(len == 0)
    {
      break;
    }else {
      perror("read");
    }
  }
#endif
  int size = lseek(fd, 0, SEEK_END); // 计算文件大小
  sendfile(cfd, fd, nullptr, size);

  return 0;
}
int sendHeadMsg(int cfd, int status, const char *descr, const char *type,
                int length) {
  // 状态行
  char buf[4096] = {0};
  sprintf(buf, "http/1.1 %d %s\r\n", status, descr);
  // 响应头
  sprintf(buf + strlen(buf), "content-type: %s\r\n", type);
  sprintf(buf + strlen(buf), "content-length: %d\r\n\r\n", length);
  send(cfd, buf, strlen(buf), 0);

  return 0;
}

const char *getFileType(const char *name) {
  // 从后往前找第一个.
  const char *dot = strrchr(name, '.');
  if (dot == NULL)
    return "text/plain; charset=utf-8"; // 纯文本
  if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
    return "text/html; charset=utf-8";
  if (strcmp(dot, ".css") == 0)
    return "text/css";
  if (strcmp(dot, ".xml") == 0)
    return "text/xml";
  if (strcmp(dot, ".txt") == 0)
    return "text/plain";
  if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
    return "image/jpeg";
  if (strcmp(dot, ".png") == 0)
    return "image/png";
  if (strcmp(dot, ".gif") == 0)
    return "image/gif";
  if (strcmp(dot, ".pdf") == 0)
    return "application/pdf";
  if (strcmp(dot, ".json") == 0)
    return "application/json";
  if (strcmp(dot, ".au") == 0)
    return "audio/basic";
  if (strcmp(dot, ".wav") == 0)
    return "audio/wav";
  if (strcmp(dot, ".avi") == 0)
    return "video/x-msvideo";
  if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
    return "video/quicktime";
  if (strcmp(dot, ".js") == 0)
    return "application/javascript";

  return "text/plain; charset=utf-8";
}
