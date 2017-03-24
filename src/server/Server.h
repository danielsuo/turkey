#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"
#include "general_utils.h"
#include "Container.h"

namespace Turkey {

struct Client {
  struct sockaddr_in addr;
  int pid;
  int sock;

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

  void listen();

  int getPort();

  int getSocket();
  void setSocket(int sock);
  void closeSocket();

  std::string getPath();

  // TODO: manage the life cycle of this pointer better (e.g., smart pointer)
  void addClient(Client *client);
  Client *getClient(int index);

private:
  Server();
  ~Server();

  int _port;
  int _socket;
  pthread_t _listener;

  std::vector<Client *> _clients;
};

}
