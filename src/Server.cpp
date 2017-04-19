#include "Server.h"
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include <iostream>

using namespace boost::interprocess;

static constexpr int kDefaultRec = 32;
static constexpr int kSharedMemorySizeBytes = 65536;
static constexpr int kMaxTimeSeriesSize = 1024;

namespace Turkey {
namespace {
int someAlgorithm(int runnableThreads) {
  // LOL
  return 10;
}
} // anonymous
Server::Server() {
  // Delete the shared memory object if one already exists
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");

  managed_shared_memory segment(create_only, "TurkeySharedMemory",
                                kSharedMemorySizeBytes);
  ShmemAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(mutex);
    segment.construct<size_t>("DefaultRec")(kDefaultRec);
    segment.construct<RecMap>("RecMap")(std::less<boost::uuids::uuid>(),
                                        allocator);
  }
}

void Server::updateTimeSeries(size_t r) {
  if (rTimeSeries_.size() >= kMaxTimeSeriesSize) {
    rTimeSeries_.pop_front();
  }
  rTimeSeries_.push_back(r);
}

void Server::poll() {
  const auto runnableThreads = procReader_.getRunnableThreads();

  updateTimeSeries(runnableThreads);

  const auto newRec = someAlgorithm(runnableThreads);
  managed_shared_memory segment(open_only, "TurkeySharedMemory");
  named_mutex mutex(open_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(mutex);
    auto map = segment.find<RecMap>("RecMap").first;
  }
}

Server::~Server() {
  LOG(INFO) << "Quitting server";
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");
}
}
