#pragma once
#include "Common.h"

namespace Turkey {
class Client {
public:
  explicit Client();

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  int getRec() { return rec_; };

private:
  int id_;
  int rec_;
};
}
