#include "Client.h"
#include <iostream>

using namespace boost::interprocess;

namespace Turkey {
Client::Client() {
  segment_ =
      std::make_unique<managed_shared_memory>(open_only, "TurkeySharedMemory");
  mutex_ = std::make_unique<named_mutex>(open_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(*mutex_);

    // Get default recommendation to use as starting value
    const auto rec = segment_->find<int>("DefaultRec").first;
    rec_ = *rec;

    // Register client in the vector
    recVec_ = std::unique_ptr<RecVec>(segment_->find<RecVec>("RecVec").first);
    id_ = recVec_->size();
    std::cout << id_ << std::endl;
    recVec_->push_back(rec_);
  }
}
}
