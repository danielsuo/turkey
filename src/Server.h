#pragma once
#include "Common.h"
#include "ProcReader.h"

namespace Turkey {
class Server {
public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  void get() const;
  void poll();

private:
  ProcReader procReader_;
  int* defaultRec_;
  RecVec* recVec_;
};
}
