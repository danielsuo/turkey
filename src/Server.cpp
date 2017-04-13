#include "Server.h"
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

  segment_ = std::make_unique<managed_shared_memory>(
      create_only, "TurkeySharedMemory", kSharedMemorySizeBytes);

  ShmAllocator allocator(segment_->get_segment_manager());
  mutex_ = std::make_unique<named_mutex>(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(*mutex_);
    defaultRec_ = std::unique_ptr<int>(
        segment_->construct<int>("DefaultRec")(kDefaultRec));

    recVec_ = std::unique_ptr<RecVec>(
        segment_->construct<RecVec>("RecVec")(allocator));
  }
}

void Server::get() const {
  scoped_lock<named_mutex> lock(*mutex_);
  std::cout << *defaultRec_ << std::endl;
}

void Server::poll() {
  const auto runnableThreads = procReader_.getRunnableThreads();
  const auto newRec = someAlgorithm(runnableThreads);
  {
    scoped_lock<named_mutex> lock(*mutex_);
    *defaultRec_ = newRec;
  }
}

Server::~Server() {
  segment_->destroy<int>("DefaultRec");
  segment_->destroy<RecVec>("RecVec");
  shared_memory_object::remove("TurkeySharedMemory");
  named_mutex::remove("TurkeyMutex");
}
}
