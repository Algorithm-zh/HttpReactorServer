#include "HttpRequest.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <strings.h>

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
