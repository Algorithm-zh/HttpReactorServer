#include "Buffer.h"
#include <bits/types/struct_iovec.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

Buffer::Buffer(int size) {
  // 为了扩容操作所以使用malloc
  m_data = (char *)malloc(size);
  m_capacity = size;
  m_writePos = m_readPos = 0;
  memset(m_data, 0, size);
}
Buffer::~Buffer() {
  if (m_data != nullptr)
    free(m_data);
}

void Buffer::extendRoom(int size) {
  // 1.内存够用-不用扩容
  if (writeableSize() >= size)
    return;
  // 2.内存需要合并才够用-不用扩容
  // 剩余的可写的内存 + 已读的内存 >= size
  else if (writeableSize() + m_readPos >= size) {
    // 把没读的内存放到前面
    int readable = readableSize();
    // 移动内存
    memcpy(m_data, m_data + m_readPos, readable);
    // 更新readPos和writePos
    m_readPos = 0;
    m_writePos = readable;
  }
  // 3.内存不够用-需要扩容
  else {
    char *tmp = (char *)realloc(m_data, m_capacity + size);
    if (tmp == nullptr)
      return;
    memset(tmp + m_capacity, 0, size);
    m_data = tmp;
    m_capacity += size;
  }
}

int Buffer::appendString(const char *data, int size) {
  if (data == nullptr || size <= 0)
    return -1;
  // 扩容(or判断内存够用否)
  extendRoom(size);
  // 拷贝
  memcpy(m_data + m_writePos, data, size);
  m_writePos += size;
  return 0;
}

int Buffer::appendString(const char *data) {
  int size = strlen(data);
  int ret = appendString(data, size);
  return ret;
}

int Buffer::appendString(const std::string data) {
  int ret = appendString(data);
  return ret;
}

int Buffer::socketRead(int fd) {
  struct iovec vec[2];
  int writeable = writeableSize();
  vec[0].iov_base = m_data + m_writePos;
  vec[0].iov_len = writeable;
  char *tmpbuf = (char *)malloc(40960);
  vec[1].iov_base = tmpbuf;
  vec[1].iov_len = 40960;
  int result = readv(fd, vec, 2);
  if (result == -1)
    return -1;
  else if (result <= writeable) {
    // buffer够用直接移动指针
    m_writePos += result;
  } else {
    // buffer不够用，被写到了tmpbuf里了，写到buf里并且需要进行扩容
    writeable = m_capacity; // buf已经被写满
    appendString(tmpbuf, result - writeable);
  }
  free(tmpbuf);
  return result;
}

char *Buffer::findCRLF() {
  // strstr. 大字符串中匹配子字符串（遇到\0结束）
  // memmem. 大数据快中匹配子数据块（需要指定各数据块大小,所以不会遇到\0结束）
  void *ptr = memmem(m_data + m_readPos, readableSize(), "\r\n", 2);
  return (char *)ptr;
}

int Buffer::sendData(int socket) {
  // 判断有无数据
  int readable = readableSize();
  if (readable > 0) {
    int count = send(socket, m_data + m_readPos, readable, 0);
    if (count > 0) {
      m_readPos += count;
      usleep(1);
    }
    return count;
  }
  return 0;
}
