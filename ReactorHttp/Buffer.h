#pragma once

#include <string>
class Buffer {
public:
  Buffer(int size);
  ~Buffer();
  void extendRoom(int size);
  // 剩余内存容量获取
  // 得到剩余的可写的内存容量
  inline int writeableSize() { return m_capacity - m_writePos; }
  // 得到剩余的可读的内存容量
  inline int readableSize() { return m_writePos - m_readPos; }
  // 写内存1.直接写 2.接收套接字数据
  int appendString(const char *data, int size);
  // 就是上面那个函数的简单版
  int appendString(const char *data);
  int appendString(const std::string data);
  int socketRead(int fd);
  // 根据\r\n取出一行，找到其在数据块中的位置，返回该位置
  char *findCRLF();
  // 发送数据
  int sendData(int socket);

  inline char *beginRead() { return m_data + m_readPos; };
  inline char *beginWrite() { return m_data + m_writePos; };
  inline int readPosIncrease(int count) {
    m_readPos += count;
    return m_readPos;
  }

private:
  // 指向内存的指针
  char *m_data;
  int m_capacity;
  int m_readPos;
  int m_writePos;
};
