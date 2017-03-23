#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "messages.h"
#include "general_utils.h"
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

  void listen();
  void launch(std::string conf);

  int getPort();

  int getSocket();
  void setSocket(int sock);
  void closeSocket();

  std::string getPath();

private:
  Server();
  ~Server();

  int _port;
  int _socket;
  pthread_t _listener;

  std::vector<Container> _containerList;
  std::mutex _containerListMutex;
};

}
