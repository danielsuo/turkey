#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <iostream>
#include <unordered_map>

#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <sys/wait.h>

#include <libcgroup.h>
#include <czmq.h>

#include "fbs/fbs.h"
#include "common/common.h"
#include "cgroup/cgroup.h"
#include "utils/general.h"

namespace Turkey {

// static const std::string TURKEY_SERVER_WORKERS_ADDR = "inproc://workers";
// static const int TURKEY_SERVER_NUM_WORKERS = 5;

struct Client {
  pid_t pid;
  turkey_shm *tshm;

  char *cg_name;
  struct cgroup *cg;
  struct cgroup_controller *cg_ctl_cpu;

  Client(pid_t pid);
  ~Client();

  void updateCPUShares(int cpu_shares);
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

  std::thread spawn(const std::string exec_path,
                    const std::vector<std::string> args = std::vector<std::string>(),
                    const std::vector<std::string> envp = std::vector<std::string>()
                  );
  // void listen(int port = TURKEY_SERVER_PORT);

private:
  // void bind(int port = TURKEY_SERVER_PORT);
  // void worker();

  void spawn_helper(const std::string exec_path,
                    const std::vector<std::string> args = std::vector<std::string>(),
                    const std::vector<std::string> envp = std::vector<std::string>()
                  );

  // TODO: manage the life cycle of this pointer better (e.g., smart pointer)
  void addClient(pid_t pid);
  void removeClient(pid_t pid);
  Client *getClient(pid_t pid);

  void updateClientCPUShares(pid_t pid, int cpu_shares);

  Server();
  ~Server();

  // zmq::context_t _context;
  // void *_context;
  std::unordered_map<pid_t, Client *> _clients;
  std::mutex _clientsMutex;
};

}
