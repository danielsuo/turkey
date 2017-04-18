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
  shared_memory_object::remove(TURKEY_SHM_KEY);

  managed_shared_memory segment(create_only, TURKEY_SHM_KEY,
                                kSharedMemorySizeBytes);
  ShmAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(mutex);
    defaultRec_ = segment.construct<size_t>("DefaultRec")(kDefaultRec);
    recVec_ = segment.construct<RecVec>("RecVec")(allocator);
  }

  named_mutex idMutex(create_only, TURKEY_ID_MUTEX_KEY);
  {
    scoped_lock<named_mutex> lock(idMutex);
    auto lastIDSeen = segment.construct<int>(TURKEY_ID_KEY)(-1);
    lastIDSeen_ = *lastIDSeen;
  }
}

void Server::poll() {
  std::cout << "Server is polling..." << std::endl;
  const auto runnableThreads = procReader_.getRunnableThreads();
  const auto newRec = someAlgorithm(runnableThreads);
  managed_shared_memory segment(open_only, TURKEY_SHM_KEY);
  named_mutex mutex(open_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(mutex);
    auto defaultRec = segment.find<size_t>("DefaultRec").first;
    *defaultRec = newRec;
  }

  auto lastIDSeen = getLastIDSeen();

  // If there are new clients
  while (lastIDSeen > lastIDSeen_) {
    lastIDSeen_++;
    LOG(INFO) << "Saw new client " << lastIDSeen_;
  }
}

Server::~Server() {
  LOG(INFO) << "Quitting server";
  named_mutex::remove("TurkeyMutex");
  named_mutex::remove(TURKEY_ID_MUTEX_KEY);
  shared_memory_object::remove(TURKEY_SHM_KEY);
}
}
