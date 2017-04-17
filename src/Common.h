#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <utility>

typedef boost::interprocess::allocator<
    size_t, boost::interprocess::managed_shared_memory::segment_manager>
    ShmAllocator;
typedef boost::interprocess::vector<size_t, ShmAllocator> RecVec;
