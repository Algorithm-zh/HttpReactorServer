#include "HttpRequest.h"
#include <cstdlib>

#define HeaderSize 12
HttpRequest::HttpRequest() {
  // 从请求行开始解析
  curState = ParseReqLine;
  method = nullptr;
  url = nullptr;
  version = nullptr;
  reqHeaders = (RequestHeader *)malloc(sizeof(RequestHeader) * HeaderSize);
  reqHeadersNum = 0;
}
HttpRequest *HttpRequest::httpRequestInit() { return new HttpRequest(); }
