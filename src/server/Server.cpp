#include "Server.h"

namespace Turkey {

void signal_handler(int signal, siginfo_t *siginfo, void *context);
void tcp_client_handler(Server *server);
void tcp_client_accept_handler(Server *server, int pid);

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

  _port = TURKEY_SERVER_PORT;
}

Server::~Server() {
  fprintf(stderr, "Destructing Server.\n");

  // TODO: C++11ify this
  for (int i = 0; i < _clients.size(); i++) {
    delete _clients[i];
  }
}

void Server::listen() {
  std::thread listener(tcp_client_handler, this);
  listener.join();
}

int Server::getPort() {
  return _port;
}

int Server::getSocket() {
  return _socket;
}

void Server::setSocket(int sock) {
  _socket = sock;
}

void Server::closeSocket() {
  close(_socket);
}

std::string Server::getPath() {
  return TURKEY_SERVER_PATH;
}

// TODO: http://zguide.zeromq.org/c:mtserver
void tcp_client_handler(Server *self) {
  // TODO: perhaps move this into the initializer; or at least have socket on the server object
  zmq::context_t context = zmq::context_t(1);
  zmq::socket_t socket = zmq::socket_t(context, ZMQ_REP);
  std::string addr = "tcp://0.0.0.0:" + std::to_string(self->getPort());
  std::cerr << "Bound to: " << addr << std::endl;
  socket.bind(addr);

  while (1) {
    fprintf(stderr, "Waiting to receive...\n");
    zmq::message_t receiveMessage;
    socket.recv(&receiveMessage);

    fprintf(stderr, "message size: %d\n", receiveMessage.size());
    fprintf(stderr, "raw: %s\n", receiveMessage.data());
    auto msg = Getturkey_msg_register_client(receiveMessage.data());
    fprintf(stderr, "parsed: %d\n", msg->pid());

    // TODO: Move this
    zmq::message_t reply(6);
    memcpy((void *)reply.data(), "World", 6);
    socket.send(reply);

    std::thread client(tcp_client_accept_handler, self, msg->pid());

    // TODO: Figure out how to wait on a number of threads
    client.join();
  }

  return 0;
}

Client::~Client() {
  close(sock);
  turkey_shm_destroy(tshm);
}

void Server::addClient(Client *client) {
  _clients.push_back(client);
}

Client *Server::getClient(int index) {
  return _clients[index];
}

void tcp_client_accept_handler(Server *self, int pid) {
  Client *client = new Client();
  client->pid = pid;
  self->addClient(client);

  client->tshm = turkey_shm_init(client->pid);

  char c;
  unsigned char *s = client->tshm->shm;

  for (c = 'a'; c <= 'z'; c++) {
    putchar(c);
    *s++ = c;
  }
  *s = NULL;
  putchar('\n');

  char buffer[1] = {'T'};
  if (write(client->sock, buffer, 1) < 0) {
    pexit("Failed to to write to client");
  }

  while (*client->tshm->shm != '*') {
    sleep(1);
  }

  return 0;
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
