#pragma once
#include "Channel.h"
#include <unordered_map>

class ChannelMap {
public:
  ChannelMap();
  static ChannelMap *ChannelMapInit();
  void ChannelMapClear(ChannelMap *map);

private:
  std::unordered_map<int, Channel *> mChannelMap;
};
