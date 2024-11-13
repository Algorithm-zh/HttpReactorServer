#include "Buffer.h"
#include <bits/types/struct_iovec.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/uio.h>

Buffer::Buffer(int size) {
  data = (char *)malloc(size);
  capacity = size;
  writePos = readPos = 0;
  memset(data, 0, size);
}
Buffer *Buffer::bufferInit(int size) { return new Buffer(size); }
void Buffer::bufferDestory() {
  if (data != nullptr)
    free(data);
}
int Buffer::bufferWriteableSize() { return capacity - writePos; }
int Buffer::bufferReadableSize() { return writePos - readPos; }
void Buffer::bufferExtendRoom(int size) {
  // 1.内存够用-不用扩容
  if (bufferWriteableSize() >= size)
    return;
  // 2.内存需要合并才够用-不用扩容
  // 剩余的可写的内存 + 已读的内存 >= size
  else if (bufferWriteableSize() + readPos >= size) {
    // 把没读的内存放到前面
    int readable = bufferReadableSize();
    // 移动内存
    memcpy(data, data + readPos, readable);
    // 更新readPos和writePos
    readPos = 0;
    writePos = readable;
  }
  // 3.内存不够用-需要扩容
  else {
    char *tmp = (char *)realloc(data, capacity + size);
    if (tmp == nullptr)
      return;
    memset(tmp + capacity, 0, size);
    data = tmp;
    capacity += size;
  }
}
int Buffer::bufferAppendData(const char *data, int size) {
  if (data == nullptr || size <= 0)
    return -1;
  // 扩容(or判断内存够用否)
  bufferExtendRoom(size);
  // 拷贝
  memcpy(this->data + writePos, data, size);
  writePos += size;
  return 0;
}
int Buffer::bufferAppendString(const char *data) {
  int size = strlen(data);
  int ret = bufferAppendData(data, size);
  return ret;
}
int Buffer::bufferSocketRead(int fd) {
  struct iovec vec[2];
  int writeable = bufferWriteableSize();
  vec[0].iov_base = data + writePos;
  vec[0].iov_len = writeable;
  char *tmpbuf = (char *)malloc(40960);
  vec[1].iov_base = tmpbuf;
  vec[1].iov_len = 40960;
  int result = readv(fd, vec, 2);
  if (result == -1)
    return -1;
  else if (result <= writeable) {
    // buffer够用直接移动指针
    writePos += result;
  } else {
    // buffer够用，被写到了tmpbuf里了，写到buf里并且需要进行扩容
    writeable = capacity; // buf已经被写满
    bufferAppendData(tmpbuf, result - writeable);
  }
  free(tmpbuf);
  return result;
}
