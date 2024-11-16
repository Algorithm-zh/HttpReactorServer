#pragma once
#include <functional>

// 定义函数指针
// 这种写法就是一个函数指针，只能接受一个普通函数，即非类成员函数，或类静态函数
// using handleFunc = int(*)(void*);
// 可调用对象包装器，只要传入的是可调用对象都可以
using handleFunc = std::function<int(void *)>;
// 强类型枚举
enum class FDEvent { TimeOut = 0x01, ReadEvent = 0x02, WriteEvent = 0x04 };
//
class Channel {

public:
  Channel(int fd, FDEvent events, handleFunc readCallback,
          handleFunc writeCallback, handleFunc destroyCallback, void *arg);
  // 修改fd的写事件（检测or不检测）（读事件一定要检测）
  // 默认情况可以不检测fd的写事件(因为写缓冲区肯定有空间可写)
  // 当要发送数据的时候，添加写事件的检测，它就会立马被触发
  void writeEventEnable(bool flag);
  // 判断是否需要检测文件描述符的写事件
  bool isWriteEventEnable();
  // 内联函数，提高代码执行效率，坏处需要更多内存
  inline int getFd() { return m_fd; }
  inline int getEvents() { return m_events; };
  inline const void *getArg() { return m_arg; }

  handleFunc readCallback;
  handleFunc writeCallback;
  handleFunc destroyCallback;

private:
  int m_fd;
  int m_events;
  void *m_arg;
};
