#include "Pool.h"
#include <glog/logging.h>
#include <iostream>
namespace Turkey {

DynamicThreadPool::DynamicThreadPool(size_t defaultNumThreads)
    : currentNumThreads_(defaultNumThreads), client_(defaultNumThreads),
      pool_(defaultNumThreads) {}

void DynamicThreadPool::updatePoolSize() {
  size_t numThreads;
  try {
    numThreads = client_.getRec();
  } catch (const std::exception& ex) {
    LOG(ERROR) << "Error in Turkey Client: " << ex.what();
    return;
  }

  if (numThreads == currentNumThreads_) {
    // Shortcircuit to avoid locking the thread list
    return;
  }

  pool_.setNumThreads(numThreads);
}
}
