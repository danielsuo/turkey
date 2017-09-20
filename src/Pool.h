#pragma once
#include <iostream>
#include <thread>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include "Client.h"

namespace Turkey {
class DynamicThreadPool {
public:
  explicit DynamicThreadPool(size_t defaultNumThreads);

  void start();
  void stop();
  size_t updatePoolSize();
  wangle::CPUThreadPoolExecutor& getPool() { return pool_; }

private:
  size_t currentNumThreads_;
  Client client_;
  wangle::CPUThreadPoolExecutor pool_;
  std::unique_ptr<std::thread> clientThread_;
};
}
