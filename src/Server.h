#pragma once
#include "Common.h"
#include "ProcReader.h"

namespace Turkey {

// Client meta data
class ClientInfo {};

// Hold stats for one client from one poll
class ClientStats {};

// Hold stats for system from one poll
class SystemStats {};

class Server {
public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  void poll();

private:
  void pollClientStats();
  void pollSystemStats();

  int lastIDSeen_;

  ProcReader procReader_;
  size_t* defaultRec_;
  RecVec* recVec_;
};
}
