#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

#include "Container.h"

using namespace docker;

namespace Turkey {

class Server {
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(Server&&) = default;
  Server& operator=(Server&&) = default;

  static Server &GetInstance() {
    static Server instance;
    return instance;
  }

  void launch(std::string conf);

private:
  Server();
  ~Server();
  std::vector<Container> _containerList;
  std::mutex _containerListMutex;
};

}
