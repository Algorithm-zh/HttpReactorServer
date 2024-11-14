#include "ChannelMap.h"

ChannelMap::ChannelMap() {};
ChannelMap *ChannelMap::ChannelMapInit() { return new ChannelMap(); }
void ChannelMap::ChannelMapClear(ChannelMap *map) {
  if (map != nullptr) {
    for (auto it : map->mChannelMap) {
      if (it.second != nullptr)
        delete it.second;
    }
    map->mChannelMap.clear();
  }
}
Channel *ChannelMap::getChannel(int fd) { return mChannelMap[fd]; }
void ChannelMap::setChannelMap(int fd, Channel *channel) {
  mChannelMap.insert(std::make_pair(fd, channel));
}
int ChannelMap::ChannelFdDelete(int fd) {
  if (mChannelMap[fd] != nullptr)
    mChannelMap.erase(fd);
  return 0;
}
