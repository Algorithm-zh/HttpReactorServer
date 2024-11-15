#pragma once

#include "Buffer.h"
#include <functional>
#define ResHeaderSize 16
// 定义状态码枚举
enum HttpStatusCode {
  Unknown,
  OK = 200,
  MovedPermanently = 301,
  MovedTemporarily = 302,
  BadRequest = 400,
  NotFound = 404
};
// 定义响应的结构体
class ResponseHeader {

public:
  char key[32];
  char value[128];
};
class HttpResponse {
public:
  // 定义一个函数指针，用来组织要回复给客户端的数据块
  using responseBody = void (*)(const char *fileName, Buffer *sendBuf,
                                int socket);
  static HttpResponse *httpResponseInit();
  HttpResponse();
  // 销毁
  void httpResonseDestroy();
  // 添加响应头
  void httpResponseAddHeader(const char *key, const char *value);
  // 组织http响应数据
  void httpResonsePrepareMsg(Buffer *sendBuf, int socket);

public:
  // 状态行:状态码，状态码描述
  enum HttpStatusCode statusCode;
  char statusMsg[128];
  // 响应头
  ResponseHeader *headers;
  int headerNum;
  responseBody sendDataFunc;
  char fileName[128];
};
