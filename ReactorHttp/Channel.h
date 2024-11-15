#pragma once
#include <functional>

// 定义函数指针
using handleFunc = std::function<void(void *)>;
enum FDEvent { TimeOut = 0x01, ReadEvent = 0x02, WriteEvent = 0x04 };
class Channel {

public:
  Channel(int fd, int events, handleFunc readCallback, handleFunc writeCallback,
          handleFunc destroyCallback, void *arg);
  static Channel *channelInit(int fd, int events, handleFunc readCallback,
                              handleFunc writeCallback,
                              handleFunc destroyCallback, void *arg);
  // 修改fd的写事件（检测or不检测）（读事件一定要检测）
  // 默认情况可以不检测fd的写事件(因为写缓冲区肯定有空间可写)
  // 当要发送数据的时候，添加写事件的检测，它就会立马被触发
  void writeEventEnable(Channel *channel, bool flag);
  // 判断是否需要检测文件描述符的写事件
  bool isWriteEventEnable(Channel *channel);
  int getFd();
  int getEvents();

  handleFunc readCallback;
  handleFunc writeCallback;
  handleFunc destroyCallback;
  void *arg;

private:
  int fd;
  int events;
};
