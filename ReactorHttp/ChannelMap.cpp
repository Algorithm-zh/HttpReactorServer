#include "ChannelMap.h"

ChannelMap::ChannelMap() {};
ChannelMap *ChannelMap::ChannelMapInit() { return new ChannelMap(); }
void ChannelMap::ChannelMapClear(ChannelMap *map) {
  if (map != nullptr) {
    for (auto it : map->mChannelMap) {
      if (it.second != nullptr)
        free(it.second);
    }
    map->mChannelMap.clear();
  }
}
