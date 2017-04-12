#include "Server.h"
#include <iostream>

using namespace boost::interprocess;
static constexpr int kSharedMemorySize = 65536;

namespace Turkey {
Server::Server() {
  // Delete the shared memory object if one already exists
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");

  managed_shared_memory segment(create_only,
                                "TurkeySharedMemory", // segment name
                                kSharedMemorySize);   // segment size in bytes

  ShmAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(mutex);
    segment.construct<RecommendationMap>("RecommendationMap") // object name
        (std::less<int>(), allocator);
  }
}

void Server::get() const {
  named_mutex mutex(open_only, "TurkeyMutex");
  scoped_lock<named_mutex> lock(mutex);
  managed_shared_memory segment(open_only, "TurkeySharedMemory");
  const auto ret = segment.find<RecommendationMap>("RecommendationMap");
  std::cout << ret.second << std::endl;
}

Server::~Server() {
  shared_memory_object::remove("TurkeySharedMemory");
  named_mutex::remove("TurkeyMutex");
}
}
