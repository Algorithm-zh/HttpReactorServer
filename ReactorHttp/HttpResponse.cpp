#include "HttpResponse.h"
#include "TcpConnection.h"
#include <cstdio>
#include <cstring>
#include <strings.h>

HttpResponse *HttpResponse::httpResponseInit() { return new HttpResponse(); }
HttpResponse::HttpResponse() {
  headerNum = 0;
  headers = new ResponseHeader[ResHeaderSize];
  statusCode = Unknown;
  // 初始化数组,以下这两个函数都可以
  memset(headers, 0, sizeof(ResponseHeader) * ResHeaderSize);
  memset(fileName, 0, sizeof(fileName));
  bzero(statusMsg, sizeof(statusMsg));
  // 函数指针
  sendDataFunc = nullptr;
}

void HttpResponse::httpResonseDestroy() { delete headers; }

void HttpResponse::httpResponseAddHeader(const char *key, const char *value) {
  if (key == nullptr || value == nullptr)
    return;
  strcpy(headers[headerNum].key, key);
  strcpy(headers[headerNum].value, value);
  headerNum++;
}

void HttpResponse::httpResonsePrepareMsg(Buffer *sendBuf, int socket) {
  // 状态行
  char tmp[1024] = {0};
  sprintf(tmp, "HTTP/1.1 %d %s\r\n", statusCode, statusMsg);
  sendBuf->bufferAppendString(tmp);
  // 响应头
  for (int i = 0; i < headerNum; i++) {
    sprintf(tmp, "%s: %s\r\n", headers->key, headers->value);
    sendBuf->bufferAppendString(tmp);
  }
  // 空行
  sendBuf->bufferAppendString("\r\n");
#ifndef MSG_SEND_AUTO
  sendBuf->bufferSendData(socket);
#endif // !MSG_SEND_AUTO
  // 回复的数据
  sendDataFunc(fileName, sendBuf, socket);
}
