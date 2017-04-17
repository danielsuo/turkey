#include "Client.h"
#include <glog/logging.h>
#include <iostream>

using namespace boost::interprocess;

namespace Turkey {
Client::Client(size_t defaultRec) : rec_(defaultRec) {
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    // Get default recommendation to use as starting value
    const auto rec = segment.find<size_t>("DefaultRec").first;
    rec_ = *rec;

    // Register client in the vector
    auto recVec = segment.find<RecVec>("RecVec").first;
    id_ = recVec->size();
    recVec->push_back(rec_);
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
  }
}
}
