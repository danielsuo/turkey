#include "Server.h"
#include <glog/logging.h>
#include <iostream>

using namespace boost::interprocess;

static constexpr int kDefaultRec = 32;
static constexpr int kSharedMemorySizeBytes = 65536;

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
  ShmAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(mutex);
    defaultRec_ = segment.construct<size_t>("DefaultRec")(kDefaultRec);
    recVec_ = segment.construct<RecVec>("RecVec")(allocator);
  }
}

void Server::poll() {
  const auto runnableThreads = procReader_.getRunnableThreads();
  const auto newRec = someAlgorithm(runnableThreads);
  managed_shared_memory segment(open_only, "TurkeySharedMemory");
  named_mutex mutex(open_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(mutex);
    auto defaultRec = segment.find<size_t>("DefaultRec").first;
    *defaultRec = newRec;
  }
}

Server::~Server() {
  LOG(INFO) << "Quitting server";
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");
}
}
