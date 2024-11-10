#include "Server.h"
#include <arpa/inet.h>
#include <assert.h>
#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
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
  printf("建立连接\n");
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
  printf("解析请求\n");
  int len = 0, total = 0;
  char tmp[1024] = {0}; // 防止buf数据被覆盖
  char buf[4096] = {0};
  while ((len = recv(cfd, tmp, sizeof(tmp), 0)) > 0) {
    if (total + len < sizeof buf)
      memcpy(buf + total, tmp, len);
    total += len;
  }
  // 判读数据是否接收完毕
  if (len == -1 && errno == EAGAIN) {
    // 解析请求行
    char *pt = strstr(buf, "\r\n");
    int reqLen = pt - buf;
    buf[reqLen] = '\0';
    parseRequestLine(buf, cfd);
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
  printf("解析请求行\n");
  // 解析请求行
  char method[12];
  char path[1024];
  sscanf(line, "%[^ ] %[^ ]", method, path);
  if (strcasecmp(method, "get") != 0) {
    return -1; // 只解析get请求
  }

  decodeMsg(path, path);
  printf("path: %s\n", path);
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
    printf("发送错误文件\n");
  }
  // 判断文件类型
  if (S_ISDIR(st.st_mode)) {
    // 目录， 将目录的内容发给客户端
    sendHeadMsg(cfd, 200, "OK", getFileType(".html"), -1);
    sendDir(file, cfd);
    printf("发送目录文件\n");

  } else {
    // 文件， 将文件内容发给客户端
    sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
    sendFile(file, cfd);
    printf("发送文件内容\n");
  }

  return 0;
}
int sendFile(const char *fileName, int cfd) {
  printf("发送文件\n");

  // 1. 打开文件
  int fd = open(fileName, O_RDONLY);
  assert(fd > 0);

#if 0
  while (1) {
    char buf[1024];
    int len = read(fd, buf, sizeof buf);
    if (len > 0) {
      send(cfd, buf, len, 0);
      usleep(10); // 让数据发的慢点，以便于对端处理数据

    } else if (len == 0) {
      break;
    } else {
      perror("read");
    }
  }
#else
  off_t offset = 0;
  int size = lseek(fd, 0, SEEK_END); // 计算文件大小
  // 注意记得回到头部
  lseek(fd, 0, SEEK_SET);
  // 把数据全部发完再结束循环
  while (offset < size) {
    // 零拷贝函数，发送效率高
    int ret = sendfile(cfd, fd, &offset, size - offset); // offset会被自动修改
    if (ret == -1 && errno == EAGAIN) {
      perror("没数据\n");
    }
  }
#endif
  close(fd);
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
int sendDir(const char *dirName, int cfd) {
  char buf[4096] = {0};
  sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName);
  struct dirent **nameList;
  // alphasort (按文件名排序)
  int num =
      scandir(dirName, &nameList, nullptr, alphasort); // 返回目录下有多少个文件
  for (int i = 0; i < num; i++) {
    // 取出文件名 nameList指向的是一个指针数组
    char *name = nameList[i]->d_name;
    struct stat st;
    char subPath[1024] = {0}; // 得到实际的相对地址
    sprintf(subPath, "%s/%s", dirName, name);
    stat(subPath, &st);
    if (S_ISDIR(st.st_mode)) {
      sprintf(buf + strlen(buf),
              "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>", name,
              name, st.st_size);
    } else {

      sprintf(buf + strlen(buf),
              "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", name, name,
              st.st_size);
    }
    send(cfd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof buf);
    // 因为是指针数组，数组里存储的是指针，每个指针都指向一块内存，所以需要把每块内存都清理掉
    free(nameList[i]);
  }
  sprintf(buf, "</table></body></html>");
  send(cfd, buf, strlen(buf), 0);
  free(nameList);
  return 0;
}
int hexToDec(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10; // 16进制表示
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0;
}
void decodeMsg(char *to, char *from) {

  for (; *from != '\0'; ++to, ++from) {
    // isxdigit 判断字符是不是16进制格式，取值在0-f
    // Linux%E5%86%85%E6%A0%B8.jpg
    if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {
      printf("开始解码\n");
      // 将16进制的数 ——》 十进制 将这个数值赋值给了字符int -> char
      //  将3个字符，变成一个字符，这个字符就是原始数据
      *to = hexToDec(from[1]) * 16 + hexToDec(from[2]);

      from += 2;
    } else {
      *to = *from;
    }
  }
  *to = '\0';
}
