#pragma once
#include "Client.h"
#include <iostream>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>

namespace Turkey {
class DynamicThreadPool {
public:
  explicit DynamicThreadPool(size_t defaultNumThreads);

  wangle::CPUThreadPoolExecutor& getPool() { return pool_; }

private:
  void updatePoolSize();
  size_t currentNumThreads_;
  Client client_;
  wangle::CPUThreadPoolExecutor pool_;
};
}
