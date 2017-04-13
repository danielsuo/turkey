#pragma once
#include "Common.h"

namespace Turkey {
class Server {
public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  void get() const;

private:
  std::unique_ptr<int> defaultRec_;
  std::unique_ptr<boost::interprocess::managed_shared_memory> segment_;
  std::unique_ptr<boost::interprocess::named_mutex> mutex_;
  std::unique_ptr<RecVec> recVec_;
};
}
