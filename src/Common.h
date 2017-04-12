#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <utility>

typedef int KeyType;
typedef int MappedType;
typedef std::pair<const int, int> ValueType;
typedef boost::interprocess::allocator<
    ValueType, boost::interprocess::managed_shared_memory::segment_manager>
    ShmAllocator;
typedef boost::interprocess::map<KeyType, MappedType, std::less<KeyType>,
                                 ShmAllocator>
    RecommendationMap;
