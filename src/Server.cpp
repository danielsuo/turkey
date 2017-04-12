#include "Server.h"
#include <iostream>

using namespace boost::interprocess;
static constexpr int kSharedMemorySizeBytes = 65536;

namespace Turkey {
Server::Server() {
  // Delete the shared memory object if one already exists
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");

  segment_ = std::make_unique<managed_shared_memory>(
      create_only,
      "TurkeySharedMemory",
      kSharedMemorySizeBytes);

  ShmAllocator allocator(segment_->get_segment_manager());
  mutex_ = std::make_unique<named_mutex>(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(*mutex_);
    segment_->construct<RecommendationMap>("RecommendationMap") // object name
        (std::less<int>(), allocator);
  }
}

void Server::get() const {
  scoped_lock<named_mutex> lock(*mutex_);
  const auto ret = segment_->find<RecommendationMap>("RecommendationMap");
  std::cout << ret.second << std::endl;
}

Server::~Server() {
  // TODO define custom deleters for the members instead of handling here
  shared_memory_object::remove("TurkeySharedMemory");
  named_mutex::remove("TurkeyMutex");
}
}
