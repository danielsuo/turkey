#pragma once
#include "Common.h"

namespace Turkey {
class Client {
public:
  Client();
  ~Client();

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
}
}
