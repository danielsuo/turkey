#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <string>
#include <utility>

namespace Turkey {
const char* const TURKEY_SHM_KEY = "TurkeySharedMemory";
const char* const TURKEY_ID_KEY = "TURKEY_ID_KEY";
const char* const TURKEY_ID_MUTEX_KEY = "TURKEY_ID_MUTEX_KEY";

// Prefix to use when creating shared memory for specific client
const char* const TURKEY_CLIENT_SHM_PREFIX_KEY = "TURKEY_CLIENT_SHM_PREFIX_KEY";
const char* const TURKEY_CLIENT_MUTEX_PREFIX_KEY =
    "TURKEY_CLIENT_MUTEX_PREFIX_KEY";

typedef boost::interprocess::allocator<
    size_t, boost::interprocess::managed_shared_memory::segment_manager>
    ShmAllocator;
typedef boost::interprocess::vector<size_t, ShmAllocator> RecVec;

int getLastIDSeen();
int incrementAndGetLastIDSeen();

std::string getClientShmKey(int id);
std::string getClientMutexKey(int id);

struct ClientShmStruct {
  int id;
  bool alive;
  size_t rec;
};
}
