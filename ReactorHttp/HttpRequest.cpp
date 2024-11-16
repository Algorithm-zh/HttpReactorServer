#include "HttpRequest.h"
#include "Buffer.h"
#include "HttpResponse.h"
#include "TcpConnection.h"
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define HeaderSize 12
HttpRequest::HttpRequest() {
  // 从请求行开始解析
  httpRequestReset();
  reqHeaders = new RequestHeader[HeaderSize];
}
HttpRequest *HttpRequest::httpRequestInit() { return new HttpRequest(); }

enum HttpRequestState HttpRequest::httpRequestState() { return curState; }

void HttpRequest::httpRequestAddHeader(const char *key, const char *value) {
  reqHeaders[reqHeadersNum].key = (char *)key;
  reqHeaders[reqHeadersNum].value = (char *)value;
  reqHeadersNum++;
}

// 解析post请求时才会用到(因为会用到请求头里的内容)
char *HttpRequest::httpRequestGetHeader(const char *key) {
  for (int i = 0; i < reqHeadersNum; i++) {
    if (strncasecmp(reqHeaders[i].key, key, strlen(key)) == 0) {
      return reqHeaders[i].value;
    }
  }
  return NULL;
}

char *HttpRequest::splitRequestLine(const char *start, const char *end,
                                    const char *sub, char **ptr) {
  int lineSize = end - start;
  char *space = (char *)end;
  if (sub != nullptr) {
    space = (char *)memmem(start, lineSize, sub, strlen(sub));
    assert(space != nullptr);
  }
  int length = space - start;
  char *tmp = (char *)malloc(length + 1);
  strncpy(tmp, start, length);
  tmp[length] = '\0';
  // 想要在函数里给外部的一级指针赋值，就必须把指针的地址传进来
  *ptr = tmp;
  return space + 1;
}
bool HttpRequest::parseHttpRequestLine(Buffer *readBuf) {
  // 读出请求行
  char *end = readBuf->bufferFindCRLF();
  // 保存字符串起始地址
  char *start = readBuf->beginRead();
  // 请求行总长度
  int lineSize = end - start;

  if (lineSize) {
    // get /xxx/xx.txt http1.1
    // 解析请求行
    start = splitRequestLine(start, end, " ", &method);
    start = splitRequestLine(start, end, " ", &url);
    splitRequestLine(start, end, nullptr, &version);
    // 为解析请求头做准备
    readBuf->readPos += lineSize;
    readBuf->readPos += 2; // 跳过/r/n到下一行
    // 修改状态
    curState = ParseReqHeaders;
    return true;
  }

  return false;
}

bool HttpRequest::parseHttpRequestHeader(Buffer *readBuf) {
  char *end = readBuf->bufferFindCRLF();
  if (end != nullptr) {
    char *start = readBuf->beginRead();
    int lineSize = end - start;
    // 基于：搜索字符串
    char *middle = (char *)memmem(start, lineSize, ": ", 2);
    if (middle != nullptr) {
      char *key = (char *)malloc(middle - start + 1);
      strncpy(key, start, middle - start);
      key[middle - start] = '\0';
      char *value = (char *)malloc(end - middle - 2 + 1);
      strncpy(value, middle + 2, end - middle - 2);
      key[end - middle - 2] = '\0';

      httpRequestAddHeader(key, value);
      // 继续向下移动指针
      readBuf->readPos += lineSize;
      readBuf->readPos += 2;
    } else {
      // 请求头已经被解析完了，跳过空行
      readBuf->readPos += 2;
      // 修改解析状态,解析完毕
      // 忽略post请求，按照get请求处理(如果是post请求，请求头下面还有东西)
      curState = ParseReqDone;
    }
    return true;
  }

  return false;
}

bool HttpRequest::parseHttpRequest(Buffer *readBuf, HttpResponse *response,
                                   Buffer *sendBuf, int socket) {
  bool flag = true;
  while (curState != ParseReqDone) {
    switch (curState) {
    case ParseReqLine:
      flag = parseHttpRequestLine(readBuf);
      break;
    case ParseReqHeaders:
      flag = parseHttpRequestHeader(readBuf);
      break;
    case ParseReqBody:
      break;
    default:
      break;
    }
    if (!flag)
      return flag;
    // 判断是否解析完毕，如果完毕了需要准备回复的数据
    if (curState == ParseReqDone) {
      // 1. 根据解析出的原始数据，对客户端的请求做出处理
      processHttpRequest(response);
      // 2. 组织响应的数据块并发送给客户端
      response->httpResonsePrepareMsg(sendBuf, socket);
    }
  }
  // 状态还原，以可以继续解析后续的消息
  curState = ParseReqLine;

  return flag;
}

int HttpRequest::hexToDec(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10; // 16进制表示
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0;
}
void HttpRequest::decodeMsg(char *to, char *from) {

  for (; *from != '\0'; ++to, ++from) {
    // isxdigit 判断字符是不是16进制格式，取值在0-f
    // Linux%E5%86%85%E6%A0%B8.jpg
    if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {
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
// 处理get的http请求
bool HttpRequest::processHttpRequest(HttpResponse *response) {

  // 解析请求行
  if (strcasecmp(method, "get") != 0) {
    return -1; // 只解析get请求
  }
  decodeMsg(url, url);
  // 处理客户端请求的静态资源
  char *file = nullptr;
  if (strcmp(url, "/") == 0) {
    file = (char *)"./";
  } else {
    file = url + 1;
  }
  // 获取文件属性
  struct stat st;
  int ret = stat(file, &st);
  if (ret == -1) {
    // 文件不存在
    // 回复404
    strcpy(response->fileName, "404.html");
    response->statusCode = NotFound;
    strcpy(response->statusMsg, "Not Found");
    // 响应头
    response->httpResponseAddHeader("Content-type", getFileType(".html"));
    response->sendDataFunc = sendFile;
  }
  strcpy(response->fileName, file);
  response->statusCode = OK;
  strcpy(response->statusMsg, "OK");
  // 判断文件类型
  if (S_ISDIR(st.st_mode)) {
    // 目录， 将目录的内容发给客户端
    // 响应头
    response->httpResponseAddHeader("Content-type", getFileType(".html"));
    response->sendDataFunc = sendDir;

  } else {
    // 文件， 将文件内容发给客户端
    // 响应头
    char tmp[12] = {0};
    sprintf(tmp, "%ld", st.st_size);
    response->httpResponseAddHeader("Content-type", getFileType(file));
    response->httpResponseAddHeader("Content-length", tmp);
    response->sendDataFunc = sendFile;
  }

  return false;
}

void HttpRequest::httpRequestReset() {
  curState = ParseReqLine;
  method = nullptr;
  url = nullptr;
  version = nullptr;
  reqHeadersNum = 0;
}
// 主要为了释放内存，之所以不在上面释放是因为初始化也调用了上面
void HttpRequest::httpRequestResetEx() {
  free(url);
  free(method);
  free(version);
  if (reqHeaders != nullptr) {
    for (int i = 0; i < reqHeadersNum; i++) {
      free(reqHeaders[i].key);
      free(reqHeaders[i].value);
    }
  }
  httpRequestReset();
}

void HttpRequest::httpRequestDestroy() {
  httpRequestResetEx();
  delete reqHeaders;
}

const char *HttpRequest::getFileType(const char *name) {
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
void HttpRequest::sendFile(const char *fileName, Buffer *sendBuf, int cfd) {

  // 1. 打开文件
  int fd = open(fileName, O_RDONLY);
  assert(fd > 0);

#if 1
  while (1) {
    char buf[1024];
    int len = read(fd, buf, sizeof buf);
    if (len > 0) {
      sendBuf->bufferAppendData(buf, len);
#ifndef MSG_SEND_AUTO
      sendBuf->bufferSendData(cfd);
#endif
      // usleep(10); // 让数据发的慢点，以便于对端处理数据
    } else if (len == 0) {
      break;
    } else {
      close(fd);
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
}
void HttpRequest::sendDir(const char *dirName, Buffer *sendBuf, int cfd) {
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
    // send(cfd, buf, strlen(buf), 0);
    sendBuf->bufferAppendString(buf);
#ifndef MSG_SEND_AUTO
    sendBuf->bufferSendData(cfd);
#endif
    free(nameList);
    memset(buf, 0, sizeof buf);
    // 因为是指针数组，数组里存储的是指针，每个指针都指向一块内存，所以需要把每块内存都清理掉
    free(nameList[i]);
  }
  sprintf(buf, "</table></body></html>");
  sendBuf->bufferAppendString(buf);
#ifndef MSG_SEND_AUTO
  sendBuf->bufferSendData(cfd);
#endif
  free(nameList);
}
