#include "HttpResponse.h"
#include "Log.h"
#include "TcpConnection.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <strings.h>
#include <utility>

HttpResponse::HttpResponse() {
  m_headers.clear();
  m_statusCode = statusCode::Unknown;
  m_fileName = std::string();
  sendDataFunc = nullptr;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::addHeader(const std::string key, const std::string value) {
  if (key.empty() || value.empty())
    return;
  m_headers.insert(std::make_pair(key, value));
}

void HttpResponse::prepareMsg(Buffer *sendBuf, int socket) {
  // 状态行
  char tmp[1024] = {0};
  int code = static_cast<int>(m_statusCode);
  sprintf(tmp, "HTTP/1.1 %d %s\r\n", code, m_info.at(code).data());
  sendBuf->appendString(tmp);
  Debug("状态行");
  // 响应头
  for (auto it : m_headers) {
    sprintf(tmp, "%s: %s\r\n", it.first.data(), it.second.data());
    sendBuf->appendString(tmp);
  }
  Debug("响应头");
  // 空行
  sendBuf->appendString("\r\n");
#ifndef MSG_SEND_AUTO
  sendBuf->sendData(socket);
#endif // !MSG_SEND_AUTO
  // 回复的数据
  sendDataFunc(m_fileName, sendBuf, socket);
}
