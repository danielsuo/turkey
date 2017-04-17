#pragma once
#include "Common.h"
#include <folly/Optional.h>

namespace Turkey {
class Client {
public:
  explicit Client(size_t defaultRec);

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  size_t getRec() { return rec_; };

private:

  folly::Optional<int> id_;
  size_t rec_;
};
}
