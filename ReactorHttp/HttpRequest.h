#pragma once

#include "Buffer.h"
#include "HttpResponse.h"
#include <cstdlib>
#include <functional>
#include <map>
#include <string>

// 当前的解析状态
enum class processState : char {
  ParseReqLine,
  ParseReqHeaders,
  ParseReqBody,
  ParseReqDone
};
class HttpRequest {
public:
  HttpRequest();
  // 重置
  void reset();
  // 结束时释放内存
  void destroy();
  // 获取处理状态
  inline processState getState() { return m_curState; }
  // 添加请求头
  void addHeader(std::string key, std::string value);
  // 根据key值得到请求头的value
  std::string getHeader(std::string key);
  // 解析请求行
  bool parseRequestLine(Buffer *readBuf);
  // 解析请求头
  bool parseRequestHeader(Buffer *readBuf);
  // 解析http请求协议
  bool parseHttpRequest(Buffer *readBuf, HttpResponse *response,
                        Buffer *sendBuf, int socket);
  // 处理http请求
  bool processHttpRequest(HttpResponse *response);
  std::string decodeMsg(std::string from);
  const std::string getFileType(const std::string name);
  void sendFile(std::string fileName, Buffer *sendBuf, int cfd);
  void sendDir(std::string dirName, Buffer *sendBuf, int cfd);

  inline void setState(processState state) { m_curState = state; }

  // 作为下面参数的callback
  inline void setMethod(std::string method) { m_method = method; }
  inline void setUrl(std::string url) { m_url = url; }
  inline void setVersion(std::string version) { m_version = version; }

private:
  // 拆分请求行
  char *splitRequestLine(const char *start, const char *end, const char *sub,
                         std::function<void(std::string)> callback);
  int hexToDec(char c);

  // 请求行
  std::string m_method;
  std::string m_url;
  std::string m_version;
  std::map<std::string, std::string> m_reqHeaders;
  processState m_curState;
};
