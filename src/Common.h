#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <utility>

typedef boost::interprocess::allocator<
    std::pair<const boost::uuids::uuid, size_t>,
    boost::interprocess::managed_shared_memory::segment_manager>
    ShmemAllocator;
typedef boost::interprocess::map<boost::uuids::uuid, size_t,
                                 std::less<boost::uuids::uuid>, ShmemAllocator>
    RecMap;
