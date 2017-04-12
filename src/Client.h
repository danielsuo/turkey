#pragma once
#include "Common.h"

namespace Turkey {
class Client {
public:
  Client();
  ~Client();

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

private:
  std::unique_ptr<boost::interprocess::managed_shared_memory> segment_;
  std::unique_ptr<boost::interprocess::named_mutex> mutex_;
  std::unique_ptr<RecommendationMap> recommendationMap_;
};
}
