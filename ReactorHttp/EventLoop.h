#pragma once
#include "Dispatcher.h"
class Dispatcher;
class EventLoop {
public:
  Dispatcher *dispatcher;
  void *dispatcherData;
};
