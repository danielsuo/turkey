#include "Server.h"

using namespace boost::interprocess;
static constexpr int kSharedMemorySize = 65536;

namespace Turkey {
Server::Server() {
  // Delete the shared memory object if one already exists
  shared_memory_object::remove("TurkeySharedMemory");
  named_mutex::remove("TurkeyNamedMutex");

  managed_shared_memory segment(create_only,
                                "TurkeySharedMemory", // segment name
                                kSharedMemorySize);   // segment size in bytes

  ShmAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(mutex);
    segment.construct<RecommendationMap>("recommendationMap") // object name
        (std::less<int>(), allocator);
  }
}

Server::~Server() {
  shared_memory_object::remove("TurkeySharedMemory");
  named_mutex::remove("TurkeyMutex");
}
}
