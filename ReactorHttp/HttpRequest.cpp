#include "HttpRequest.h"
#include "Buffer.h"
#include "HttpResponse.h"
#include "Log.h"
#include "TcpConnection.h"
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <functional>
#include <string>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

#define HeaderSize 12
HttpRequest::HttpRequest() { reset(); }

void HttpRequest::reset() {
  m_curState = processState::ParseReqLine;
  m_method = m_url = m_version = std::string();
  m_reqHeaders.clear();
}

void HttpRequest::addHeader(const std::string key, const std::string value) {
  if (key.empty() || value.empty())
    return;
  m_reqHeaders.insert(std::make_pair(key, value));
}

// 解析post请求时才会用到(因为会用到请求头里的内容)
std::string HttpRequest::getHeader(const std::string key) {

  auto item = m_reqHeaders.find(key);
  if (item == m_reqHeaders.end())
    return std::string();
  return item->second;
}

char *HttpRequest::splitRequestLine(const char *start, const char *end,
                                    const char *sub,
                                    std::function<void(std::string)> callback) {
  int lineSize = end - start;
  char *space = const_cast<char *>(end);
  if (sub != nullptr) {
    space = (char *)memmem(start, lineSize, sub, strlen(sub));
    assert(space != nullptr);
  }
  int length = space - start;
  callback(std::string(start, length));
  return space + 1;
}

bool HttpRequest::parseRequestLine(Buffer *readBuf) {
  // 读出请求行
  char *end = readBuf->findCRLF();
  // 保存字符串起始地址
  char *start = readBuf->beginRead();
  // 请求行总长度
  int lineSize = end - start;
  if (lineSize) {
    // get /xxx/xx.txt http1.1
    // 解析请求行
    auto methodFunc =
        std::bind(&HttpRequest::setMethod, this, std::placeholders::_1);
    auto urlFunc = std::bind(&HttpRequest::setUrl, this, std::placeholders::_1);
    auto versionFunc =
        std::bind(&HttpRequest::setVersion, this, std::placeholders::_1);
    start = splitRequestLine(start, end, " ", methodFunc);
    start = splitRequestLine(start, end, " ", urlFunc);
    splitRequestLine(start, end, nullptr, versionFunc);
    // 为解析请求头做准备
    readBuf->readPosIncrease(lineSize + 2);
    // 修改状态
    setState(processState::ParseReqHeaders);
    return true;
  }
  return false;
}

bool HttpRequest::parseRequestHeader(Buffer *readBuf) {
  char *end = readBuf->findCRLF();
  if (end != nullptr) {
    char *start = readBuf->beginRead();
    int lineSize = end - start;
    // 基于：搜索字符串
    char *middle = (char *)memmem(start, lineSize, ": ", 2);
    if (middle != nullptr) {
      int keyLen = middle - start;
      int valueLen = end - middle - 2;
      if (keyLen > 0 && valueLen > 0) {
        std::string key(start, keyLen);
        std::string value(middle + 2, valueLen);
        addHeader(key, value);
      }
      // 继续向下移动指针
      readBuf->readPosIncrease(lineSize + 2);
    } else {
      // 请求头已经被解析完了，跳过空行
      readBuf->readPosIncrease(2);
      // 修改解析状态,解析完毕
      // 忽略post请求，按照get请求处理(如果是post请求，请求头下面还有东西)
      setState(processState::ParseReqDone);
    }
    return true;
  }
  return false;
}

bool HttpRequest::parseHttpRequest(Buffer *readBuf, HttpResponse *response,
                                   Buffer *sendBuf, int socket) {
  bool flag = true;
  while (m_curState != processState::ParseReqDone) {
    switch (m_curState) {
    case processState::ParseReqLine:
      flag = parseRequestLine(readBuf);
      Debug("解析请求行");
      break;
    case processState::ParseReqHeaders:
      flag = parseRequestHeader(readBuf);
      Debug("解析请求头");
      break;
    case processState::ParseReqBody:
      break;
    default:
      break;
    }
    if (!flag)
      return flag;
    // 判断是否解析完毕，如果完毕了需要准备回复的数据
    if (m_curState == processState::ParseReqDone) {
      // 1. 根据解析出的原始数据，对客户端的请求做出处理
      processHttpRequest(response);
      Debug("准备发送数据");
      // 2. 组织响应的数据块并发送给客户端
      response->prepareMsg(sendBuf, socket);
      Debug("发送数据");
    }
  }
  // 状态还原，以可以继续解析后续的消息
  m_curState = processState::ParseReqLine;
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
std::string HttpRequest::decodeMsg(std::string msg) {

  std::string str = std::string();
  const char *from = msg.data();
  for (; *from != '\0'; ++from) {
    // isxdigit 判断字符是不是16进制格式，取值在0-f
    // Linux%E5%86%85%E6%A0%B8.jpg
    if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {
      // 将16进制的数 ——》 十进制 将这个数值赋值给了字符int -> char
      //  将3个字符，变成一个字符，这个字符就是原始数据
      str.append(1, hexToDec(from[1]) * 16 + hexToDec(from[2]));
      from += 2;
    } else {
      str.append(1, *from);
    }
  }
  str.append(1, '\0');
  return str;
}

// 处理get的http请求
bool HttpRequest::processHttpRequest(HttpResponse *response) {

  // 解析请求行
  // data方法将string转化为const char*类型
  if (strcasecmp(m_method.data(), "get") != 0) {
    return -1; // 只解析get请求
  }
  m_url = decodeMsg(m_url);
  // 处理客户端请求的静态资源
  const char *file = nullptr;
  if (strcmp(m_url.data(), "/") == 0) {
    file = "./";
  } else {
    file = m_url.data() + 1;
  }
  // 获取文件属性
  struct stat st;
  int ret = stat(file, &st);
  auto obj1 = std::bind(&HttpRequest::sendFile, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3);
  if (ret == -1) {
    // 文件不存在
    // 回复404
    response->setFileName("404.html");
    response->setStatusCode(statusCode::NotFound);
    // 响应头
    response->addHeader("Content-type", getFileType(".html"));
    response->sendDataFunc = obj1;
  }
  response->setFileName(file);
  response->setStatusCode(statusCode::OK);
  // 判断文件类型
  if (S_ISDIR(st.st_mode)) {
    // 目录， 将目录的内容发给客户端
    // 响应头
    response->addHeader("Content-type", getFileType(".html"));
    auto obj2 = std::bind(&HttpRequest::sendDir, this, std::placeholders::_1,
                          std::placeholders::_2, std::placeholders::_3);
    response->sendDataFunc = obj2;

  } else {
    // 文件， 将文件内容发给客户端
    // 响应头
    response->addHeader("Content-type", getFileType(file));
    response->addHeader("Content-length", std::to_string(st.st_size));
    response->sendDataFunc = obj1;
  }

  return false;
}

const std::string HttpRequest::getFileType(const std::string name) {
  // 从后往前找第一个.
  const char *dot = strrchr(name.data(), '.');
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

void HttpRequest::sendFile(std::string fileName, Buffer *sendBuf, int cfd) {

  // 1. 打开文件
  int fd = open(fileName.data(), O_RDONLY);
  assert(fd > 0);

#if 1
  while (1) {
    char buf[1024];
    int len = read(fd, buf, sizeof buf);
    if (len > 0) {
      sendBuf->appendString(buf, len);
#ifndef MSG_SEND_AUTO
      sendBuf->sendData(cfd);
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
void HttpRequest::sendDir(std::string dirName, Buffer *sendBuf, int cfd) {
  char buf[4096] = {0};
  sprintf(buf, "<html><head><title>%s</title></head><body><table>",
          dirName.data());
  struct dirent **nameList;
  // alphasort (按文件名排序)
  // 返回目录下有多少个文件
  int num = scandir(dirName.data(), &nameList, nullptr, alphasort);
  for (int i = 0; i < num; i++) {
    // 取出文件名 nameList指向的是一个指针数组
    char *name = nameList[i]->d_name;
    struct stat st;
    char subPath[1024] = {0}; // 得到实际的相对地址
    sprintf(subPath, "%s/%s", dirName.data(), name);
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
    sendBuf->appendString(buf);
#ifndef MSG_SEND_AUTO
    sendBuf->sendData(cfd);
#endif
    memset(buf, 0, sizeof buf);
    // 因为是指针数组，数组里存储的是指针，每个指针都指向一块内存，所以需要把每块内存都清理掉
    free(nameList[i]);
  }
  sprintf(buf, "</table></body></html>");
  sendBuf->appendString(buf);
#ifndef MSG_SEND_AUTO
  sendBuf->sendData(cfd);
#endif
  free(nameList);
}
