#include "Server.h"

namespace Turkey {

void signal_handler(int signal, siginfo_t *siginfo, void *context);

Server::Server() {
  // Create sigaction struct
  struct sigaction action;

  // Zero-out sigaction struct
  memset(&action, 0, sizeof(action));

  // Set sigaction signal_handler
  action.sa_sigaction = &signal_handler;

  // Request handler to receive siginfo
  action.sa_flags = SA_SIGINFO;

  // Attach sigaction to signal
  if (sigaction(SIGINT, &action, NULL) < 0) {
    perror("Failed to attach sigaction to SIGINT in Turkey server");
    exit(1);
  }

  _context = zmq_ctx_new();
}

Server::~Server() {
  fprintf(stderr, "Destructing Server.\n");

  // TODO: C++11ify this
  for (int i = 0; i < _clients.size(); i++) {
    delete _clients[i];
  }

  zmq_ctx_destroy(_context);
}

void Server::spawn(const std::string exec_path, const std::vector<std::string> args) {
  Client *client = new Client();

  char **cargs = new char *[args.size()];

  for (size_t i = 0; i < args.size(); i++) {
    cargs[i] = new char[args[i].size() + 1];
    strcpy(cargs[i], args[i].c_str());
  }

  client->pid = fork();

  // Child proces
  if (client->pid == 0) {
    fprintf(stderr, "In child!\n");
    if (execv(exec_path.c_str(), cargs) < 0) {
      pexit("Failed to execv");
    }
  }

  // Fork error'd
  else if (client->pid == -1) {
    pexit("Failed to fork child process");
  }

  // Parent process
  else if (client->pid > 0) {
    fprintf(stderr, "In parent!\n");
    int child_status;
    pid_t wait_pid = wait(&child_status);
    if (wait_pid == -1) {
      pexit("Failed to wait for child process");
    } else {
      if (WIFEXITED(child_status)) {
        int ret = WEXITSTATUS(child_status);
        fprintf(stderr, "Child existed normally with %d\n", ret);
      }
    }
  }

  fprintf(stderr, "Freeing memory\n");
  for (size_t i = 0; i < args.size(); i++) {
    delete [] cargs[i];
  }
  delete [] cargs;
}

void Server::listen(int port) {
  std::thread listener(&Server::bind, this, port);
  listener.join();
}

void Server::bind(int port) {
  // Use Router-Dealer pattern: http://zguide.zeromq.org/cpp:mtserver

  // ROUTER (get client requests)

  // Create socket
  std::string addr = "tcp://*:" + std::to_string(port);
  void *clients = zmq_socket(_context, ZMQ_ROUTER);
  zmq_bind(clients, addr.c_str());
  std::cerr << "Clients bound to: " << addr << std::endl;

  // DEALER (manage workers)
  void *workers = zmq_socket(_context, ZMQ_DEALER);
  zmq_bind(workers, TURKEY_SERVER_WORKERS_ADDR.c_str());
  std::cerr << "Workers bound to: " << TURKEY_SERVER_WORKERS_ADDR << std::endl;

  std::vector<std::thread> worker_threads;
  for (int i = 0; i < TURKEY_SERVER_NUM_WORKERS; i++) {
    worker_threads.emplace_back(&Server::worker, this);
  }

  // Connect clients to workers
  // TODO: check return code
  zmq_proxy(clients, workers, NULL);

  for (auto &worker_thread : worker_threads) {
    worker_thread.join();
  }

  zmq_close(clients);
  zmq_close(workers);
}

void Server::worker() {
  zsock_t *socket = zmq_socket(_context, ZMQ_REP);
  zmq_connect(socket, TURKEY_SERVER_WORKERS_ADDR.c_str());
  if (socket == NULL) {
    pexit("Failed to create socket in worker thread");
  }
  fprintf(stderr, "Created worker socket!\n");


  while (true) {
    fprintf(stderr, "Waiting for message...\n");
    zmsg_t *msg = zmsg_recv(socket);
    if (msg == NULL) {
      pexit("Failed to receive message");
    }

    fprintf(stderr, "message size: %d\n", zmsg_size(msg));
    zframe_t *frame = zmsg_pop(msg);
    void *raw = zframe_data(frame);
    fprintf(stderr, "raw: %s\n", raw);
    auto data = Getturkey_msg_register_client(raw);
    fprintf(stderr, "parsed: %d\n", data->pid());

    Client *client = new Client();
    client->pid = data->pid();
    addClient(client);

    client->tshm = turkey_shm_init(client->pid);

    flatbuffers::FlatBufferBuilder fbb;
    turkey_shm_cpuBuilder builder(fbb);
    builder.add_shares(512);

    auto response = builder.Finish();
    fbb.Finish(response);

    if (turkey_shm_write(client->tshm, fbb.GetBufferPointer(), fbb.GetSize()) < 0) {
      pexit ("Failed to write to shared memory");
    }

    zmsg_t *reply = zmsg_new();
    zframe_t *reply_frame = zframe_new("OK", 2);
    zmsg_append(reply, &reply_frame);
    zmsg_send(&reply, socket);
  }

  zmq_close(socket);
}

void Server::addClient(Client *client) {
  _clients.push_back(client);
}

Client *Server::getClient(int index) {
  return _clients[index];
}

Client::~Client() {
  turkey_shm_destroy(tshm);
}

void signal_handler(int signal, siginfo_t *siginfo, void *context) {
  printf ("Sending PID: %ld, UID: %ld\n",
			(long)siginfo->si_pid, (long)siginfo->si_uid);

  // If we send a signal from ourselves (e.g., KILL), then exit
  if ((long)siginfo->si_pid == 0) {
    exit(1);
  }
}

}
