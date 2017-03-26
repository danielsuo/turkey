#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <iostream>

#include <signal.h>
#include <unistd.h>

#include <string.h>

#include <czmq.h>

#include "fbs/fbs.h"
#include "common/common.h"
#include "utils/general.h"
#include "docker/Container.h"

namespace Turkey {

static const std::string TURKEY_SERVER_WORKERS_ADDR = "inproc://workers";
static const int TURKEY_SERVER_NUM_WORKERS = 5;

struct Client {
  int pid;
  struct turkey_shm *tshm;

  ~Client();
};

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

  void listen(int port = TURKEY_SERVER_PORT);

private:
  void bind(int port = TURKEY_SERVER_PORT);
  void worker();

  // TODO: manage the life cycle of this pointer better (e.g., smart pointer)
  void addClient(Client *client);
  Client *getClient(int index);

  Server();
  ~Server();

  // zmq::context_t _context;
  void *_context;
  std::vector<Client *> _clients;
};

}
