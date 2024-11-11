#include "EventLoop.h"
#include "ChannelMap.h"
#include "EpollDispatcher.h"

EventLoop::EventLoop() : isQuit(false) {
  dispatcher = new EpollDispatcher();
  mChannelMap = ChannelMap::ChannelMapInit();
}

EventLoop *EventLoop::eventLoopInit() { return new EventLoop(); }
