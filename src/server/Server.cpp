#include "Server.h"

namespace Turkey {

static void handler(int signal, siginfo_t *siginfo, void *context) {
  printf ("Sending PID: %ld, UID: %ld\n",
			(long)siginfo->si_pid, (long)siginfo->si_uid);

  // If we send a signal from ourselves (e.g., KILL), then exit
  if ((long)siginfo->si_pid == 0) {
    exit(1);
  }
}

Server::Server() {
  // Create sigaction struct
  struct sigaction action;

  // Zero-out sigaction struct
  memset(&action, 0, sizeof(action));

  // Set sigaction handler
  action.sa_sigaction = &handler;

  // Request handler to receive siginfo
  action.sa_flags = SA_SIGINFO;

  // Attach sigaction to signal
  if (sigaction(SIGINT, &action, NULL) < 0) {
    perror("Failed to attach sigaction to SIGINT in Turkey server");
    exit(1);
  }
}

Server::~Server() {
  fprintf(stderr, "Destructing Server.\n");
}

void Server::launch(std::string conf) {
  std::lock_guard<std::mutex> lock(_containerListMutex);
  _containerList.emplace_back(conf);
  _containerList.back().attach();
  _containerList.back().start();
  Response res = _containerList.back().logs();

  std::cout << res.data << std::endl;

}

}
