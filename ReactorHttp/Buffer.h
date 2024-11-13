#pragma once

class Buffer {
public:
  Buffer(int size);
  static Buffer *bufferInit(int size);
  void bufferDestory();
  void bufferExtendRoom(int size);
  // 剩余内存容量获取
  // 得到剩余的可写的内存容量
  int bufferWriteableSize();
  // 得到剩余的可读的内存容量
  int bufferReadableSize();
  // 写内存1.直接写 2.接收套接字数据
  int bufferAppendData(const char *data, int size);
  // 就是上面那个函数的简单版
  int bufferAppendString(const char *data);
  int bufferSocketRead(int fd);

private:
  // 指向内存的指针
  char *data;
  int capacity;
  int readPos;
  int writePos;
};
