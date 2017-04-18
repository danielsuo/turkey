#pragma once
#include "Common.h"
#include <folly/Optional.h>

namespace Turkey {
class Client {
public:
  explicit Client(size_t defaultRec);
  ~Client();

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  size_t pollServer();

private:
  std::string shmKey_;
  std::string mutexKey_;
  void registerWithServer();
  folly::Optional<int> id_;
  size_t rec_;
};
}
