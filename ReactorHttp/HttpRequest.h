#pragma once

#include "Buffer.h"
#include "HttpResponse.h"
class RequestHeader {

public:
  char *key;
  char *value;
};
// 当前的解析状态
enum HttpRequestState {
  ParseReqLine,
  ParseReqHeaders,
  ParseReqBody,
  ParseReqDone
};
class HttpRequest {
public:
  HttpRequest();
  static HttpRequest *httpRequestInit();
  // 重置
  void httpRequestReset();
  // 重置时释放内存
  void httpRequestResetEx();
  // 结束时释放内存
  void httpRequestDestroy();
  // 获取处理状态
  enum HttpRequestState httpRequestState();
  // 添加请求头
  void httpRequestAddHeader(const char *key, const char *value);
  // 根据key值得到请求头的value
  char *httpRequestGetHeader(const char *key);
  // 解析请求行
  bool parseHttpRequestLine(Buffer *readBuf);
  // 拆分请求行
  char *splitRequestLine(const char *start, const char *end, const char *sub,
                         char **ptr);
  // 解析请求头
  bool parseHttpRequestHeader(Buffer *readBuf);
  // 解析http请求协议
  bool parseHttpRequest(Buffer *readBuf, HttpResponse *response,
                        Buffer *sendBuf, int socket);
  // 处理http请求
  bool processHttpRequest(HttpResponse *response);
  void decodeMsg(char *to, char *from);
  int hexToDec(char c);
  const char *getFileType(const char *name);
  static void sendFile(const char *fileName, Buffer *sendBuf, int cfd);
  static void sendDir(const char *dirName, Buffer *sendBuf, int cfd);

private:
  // 请求行
  char *method;
  char *url;
  char *version;
  // 请求头
  RequestHeader *reqHeaders;
  int reqHeadersNum;
  enum HttpRequestState curState;
};
