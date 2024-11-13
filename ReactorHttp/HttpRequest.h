#pragma once

class RequestHeader {
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
