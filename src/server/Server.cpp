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
}

Server::~Server() {
  fprintf(stderr, "Destructing Server.\n");

  // TODO: C++11ify this
  for (int i = 0; i < _clients.size(); i++) {
    delete _clients[i];
  }

  zmq_ctx_destroy(_context);
}

void Server::listen(int port) {
  std::thread listener(&Server::bind, this, port);
  listener.join();
}

void Server::bind(int port) {
  // Use Router-Dealer pattern: http://zguide.zeromq.org/cpp:mtserver
  // _context = zmq::context_t(1);
  _context = zmq_ctx_new();

  // ROUTER (get client requests)
  // zmq::socket_t clients(_context, ZMQ_ROUTER);

  // Create socket
  std::string addr = "tcp://*:" + std::to_string(port);
  // clients.bind(addr);
  // zsock_t *clients = zsock_new_router(addr.c_str());
  void *clients = zmq_socket(_context, ZMQ_ROUTER);
  zmq_bind(clients, addr.c_str());
  std::cerr << "Clients bound to: " << addr << std::endl;

  // DEALER (manage workers)
  // zmq::socket_t workers(_context, ZMQ_DEALER);
  // workers.bind(TURKEY_SERVER_WORKERS_ADDR);
  // zsock_t *workers = zsock_new_dealer(TURKEY_SERVER_WORKERS_ADDR.c_str());
  void *workers = zmq_socket(_context, ZMQ_DEALER);
  zmq_bind(workers, TURKEY_SERVER_WORKERS_ADDR.c_str());
  std::cerr << "Workers bound to: " << TURKEY_SERVER_WORKERS_ADDR << std::endl;

  std::vector<std::thread> worker_threads;
  for (int i = 0; i < TURKEY_SERVER_NUM_WORKERS; i++) {
    worker_threads.emplace_back(&Server::worker, this);
  }

  // Connect clients to workers
  // zmq::proxy(clients, workers, NULL);
  // TODO: check return code
  zmq_proxy(clients, workers, NULL);

  for (auto &worker_thread : worker_threads) {
    worker_thread.join();
  }

  zmq_close(clients);
  zmq_close(workers);
}

void Server::worker() {
  // zmq::socket_t socket(_context, ZMQ_REP);
  // socket.connect(TURKEY_SERVER_WORKERS_ADDR);

  zsock_t *socket = zmq_socket(_context, ZMQ_REP);
  zmq_connect(socket, TURKEY_SERVER_WORKERS_ADDR.c_str());
  if (socket == NULL) {
    pexit("Failed to create socket in worker thread");
  }
  fprintf(stderr, "Created worker socket!\n");


  while (true) {
    // zmq::message_t msg;
    // socket.recv(&msg);
    fprintf(stderr, "Waiting for message...\n");
    zmsg_t *msg = zmsg_recv(socket);
    if (msg == NULL) {
      pexit("Failed to receive message");
    }

    fprintf(stderr, "mesage size: %d\n", zmsg_size(msg));
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
    builder.add_share(512);

    auto response = builder.Finish();
    fbb.Finish(response);

    if (turkey_shm_lock(client->tshm) < 0) {
      pexit("Failed to lock shared memory");
    }
    memcpy(client->tshm->shm, fbb.GetBufferPointer(), fbb.GetSize());
    if (turkey_shm_unlock(client->tshm) < 0) {
      pexit("Failed to unlock shared memory");
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
