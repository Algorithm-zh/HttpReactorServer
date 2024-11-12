#pragma once
#include "Channel.h"
#include <unordered_map>

class ChannelMap {
public:
  ChannelMap();
  static ChannelMap *ChannelMapInit();
  void ChannelMapClear(ChannelMap *map);
  Channel *getChannel(int fd);
  void setChannelMap(int fd, Channel *channel);
  int ChannelFdDelete(int fd);

private:
  std::unordered_map<int, Channel *> mChannelMap;
};
