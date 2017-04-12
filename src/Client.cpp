#include "Client.h"

using namespace boost::interprocess;

namespace Turkey {
Client::Client() {
  segment_ =
      std::make_unique<managed_shared_memory>(open_only, "TurkeySharedMemory");
  mutex_ = std::make_unique<named_mutex>(open_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(*mutex_);
    auto recPair = segment_->find<RecommendationMap>("RecommendationMap");
    recommendationMap_ = std::unique_ptr<RecommendationMap>(recPair.first);
  }
}
}
