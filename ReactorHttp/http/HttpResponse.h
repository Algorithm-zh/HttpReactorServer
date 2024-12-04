#pragma once

#include "Buffer.h"
#include <functional>
#include <map>
#include <string>
#define ResHeaderSize 16
// 定义状态码枚举
enum class statusCode {
  Unknown,
  OK = 200,
  MovedPermanently = 301,
  MovedTemporarily = 302,
  BadRequest = 400,
  NotFound = 404
};

class HttpResponse {
public:
  // 定义一个函数指针，用来组织要回复给客户端的数据块
  using responseBody =
      std::function<void(std::string fileName, Buffer *sendBuf, int socket)>;
  HttpResponse();
  // 销毁
  ~HttpResponse();
  // 添加响应头
  void addHeader(const std::string key, const std::string value);
  // 组织http响应数据
  void prepareMsg(Buffer *sendBuf, int socket);
  inline void setStatusCode(statusCode statusCode) {
    m_statusCode = statusCode;
  }
  inline void setFileName(std::string fileName) { m_fileName = fileName; }

  responseBody sendDataFunc;

public:
  // 状态行:状态码,状态码描述
  statusCode m_statusCode;
  // 响应头
  std::map<std::string, std::string> m_headers;
  std::string m_fileName;
  // 定义状态码和描述
  const std::map<int, std::string> m_info = {
      {200, "OK"},         {301, "MovedPermanently"}, {302, "MovedTemporarily"},
      {400, "BadRequest"}, {404, "NotFound"},
  };
};
