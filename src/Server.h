#pragma once
#include "Common.h"
#include "ProcReader.h"
#include <queue>
namespace Turkey {
class Server {
public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  void poll();

private:
  void updateTimeSeries(size_t r);

  ProcReader procReader_;
  std::deque<size_t> rTimeSeries_;
};
}
