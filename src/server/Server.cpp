#include "Server.h"

namespace Turkey {

void signal_handler(int signal, siginfo_t *siginfo, void *context);
void *tcp_client_handler(void *server);
void *tcp_client_accept_handler(void *server);

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

  pthread_kill(_listener, SIGTERM);
}

// void Server::launch(std::string conf) {
//   std::lock_guard<std::mutex> lock(_containerListMutex);
//   _containerList.emplace_back(conf);
//   _containerList.back().attach();
//   _containerList.back().start();
//   Response res = _containerList.back().logs();
//
//   std::cout << res.data << std::endl;
// }

void Server::listen() {
  fprintf(stderr, "Listening on %d\n", _port);
  int rc = pthread_create(&_listener, NULL, tcp_client_handler, this);
  if (rc) {
    printf("ERROR: return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
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

void *tcp_client_handler(void *server) {
  Server *self = (Server *)server;

  fprintf(stderr, "In client handler!\n");

  // Stores size of the address of the client for the accept system call
  socklen_t clilen;

  // A sockaddr_in is a structure containing an internet address. This
  // structure is defined in netinet/in.h
  //
  // An in_addr structure, defined in the same header file, contains only one
  // field, a unsigned long called s_addr.
  //
  // struct sockaddr_in
  // {
  //   short   sin_family; /* must be AF_INET */
  //   u_short sin_port;
  //   struct  in_addr sin_addr;
  //   char    sin_zero[8]; /* Not used, must be zero */
  // };
  //
  // The variable serv_addr will contain the address of the server, and
  // cli_addr will contain the address of the client which connects to the
  // server.
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  // The socket() system call creates a new socket. There are three arguments:
  //
  // 1. The address domain of the socket: there are two possible address
  // domains, the unix domain for two processes which share a common file
  // system, and the Internet domain for any two hosts on the Internet. The
  // symbol constant AF_UNIX is used for the former, and AF_INET for the
  // latter (there are actually many other options which can be used here for
  // specialized purposes).
  //
  // 2. The second argument is the type of socket. Recall that there are two
  // choices here, a stream socket in which characters are read in a
  // continuous stream as if from a file or pipe, and a datagram socket, in
  // which messages are read in chunks. The two symbolic constants are
  // SOCK_STREAM and SOCK_DGRAM.
  //
  // 3. The third argument is the protocol. If this argument is zero (and it
  // always should be except for unusual circumstances), the operating system
  // will choose the most appropriate protocol. It will choose TCP for stream
  // sockets and UDP for datagram sockets.
  //
  // The socket system call returns an entry into the file descriptor table
  // (i.e. a small integer). This value is used for all subsequent references
  // to this socket.
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  // If the socket call fails, it returns -1
  if (sock < 0) {
    pexit("ERROR: Unable to open socket for listening");
  }
  self->setSocket(sock);

  // the SO_REUSEADDR option tells the kernel that when the socket is closed,
  // the port bound to the socket should be freed immediately rather than kept
  // in-use for some period of time.
  int reuseaddr_option_val = 1;
  setsockopt(self->getSocket(), SOL_SOCKET, SO_REUSEADDR, &reuseaddr_option_val, sizeof(int));

  // The function bzero() sets all values in a buffer to zero. It takes two
  // arguments, the first is a pointer to the buffer and the second is the
  // size of the buffer. Thus, this line initializes serv_addr to zeros.
  bzero((char *) &serv_addr, sizeof(serv_addr));

  // Address type is network
  serv_addr.sin_family = AF_INET;

  // Get port address in network byte order
  serv_addr.sin_port = htons(self->getPort());

  // // Copy path
  // strcpy(serv_addr.sun_path, TURKEY_SERVER_PATH);
  // unlink(serv_addr.sun_path);
  // int len = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

  // if (bind(self->getSocket(), (struct sockaddr *)&serv_addr, len) == -1) {
  //   pexit("ERROR: Unable to bind socket");
  // } else {
  //   std::cerr << "Starting server at path: " << self->getPath() << std::endl;
  // }

  // The third field of sockaddr_in is a structure of type struct in_addr
  // which contains only a single field unsigned long s_addr. This field
  // contains the IP address of the host. For server code, this will always be
  // the IP address of the machine on which the server is running, and there
  // is a symbolic constant INADDR_ANY which gets this address.
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  // The bind() system call binds a socket to an address, in this case the
  // address of the current host and port number on which the server will run.
  // It takes three arguments, the socket file descriptor, the address to
  // which is bound, and the size of the address to which it is bound. The
  // second argument is a pointer to a structure of type sockaddr, but what is
  // passed in is a structure of type sockaddr_in, and so this must be cast to
  // the correct type. This can fail for a number of reasons, the most obvious
  // being that this socket is already in use on this machine.
  if (bind(self->getSocket(), (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
  } else {
    printf("Starting server with pid %d at port %d\n", getpid(), self->getPort());
  }

  // The listen system call allows the process to listen on the socket for
  // connections. The first argument is the socket file descriptor, and the
  // second is the size of the backlog queue, i.e., the number of connections
  // that can be waiting while the process is handling a particular
  // connection. This should be set to 5, the maximum size permitted by most
  // systems.
  if (listen(self->getSocket(), 5) < 0) {
    pexit("ERROR: failed to listen");
  }

  // Get length of client address
  clilen = sizeof(cli_addr);

  // Accept new connections and data until we kill the process
  while (1) {
    // The accept() system call causes the process to block until a client
    // connects to the server. Thus, it wakes up the process when a connection
    // from a client has been successfully established. It returns a new file
    // descriptor, and all communication on this connection should be done using
    // the new file descriptor. The second argument is a reference pointer to
    // the address of the client on the other end of the connection, and the
    // third argument is the size of this structure.
    int client_socket = accept(self->getSocket(), (struct sockaddr *) &cli_addr, &clilen);

    if (client_socket < 0) {
      perror("ERROR on accept");
    }

    Client *client = new Client();
    client->addr = cli_addr;
    client->sock = client_socket;
    self->addClient(client);

    pthread_t client_processing_thread;
    if (pthread_create(&client_processing_thread, NULL, tcp_client_accept_handler, client) != 0) {
      pexit("Failed to process client");
    }

    fprintf(stderr, "Out of processing thread\n");
    // printf("Number of connected devices before: %lu\n", self->manager->devices.size());
    //
    // Device *device = self->manager->getDeviceByIPAddress(cli_addr.sin_addr.s_addr, cli_addr.sin_port);
    // device->dat_fd = client_socket;

    // printf("Number of connected devices: %lu\n", self->manager->devices.size());

    // pthread_t dat_thread;

    // int rc = pthread_create(&dat_thread, NULL, handler_client_data, (void *)device);
    // if (rc) {
    //   printf("ERROR: return code from pthread_create() is %d\n", rc);
    //   exit(-1);
    // }
    //
    // printf("\n");
  }

  // Close the sockets
  self->closeSocket();

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

void *tcp_client_accept_handler(void *client) {
  Client *self = (Client *)client;

  fprintf(stderr, "Received connection from %s\n", inet_ntoa(self->addr.sin_addr));

  int n;
  uint32_t n_pid;
  if ((n = read(self->sock, &n_pid, sizeof(n_pid))) < 0) {
    pexit("Failed to read data from server");
  }

  self->pid = ntohl(n_pid);

  fprintf(stderr, "Got pid %d (%d)\n", self->pid, n_pid);

  self->tshm = turkey_shm_init(self->pid);

  char c;
  unsigned char *s = self->tshm->shm;

  for (c = 'a'; c <= 'z'; c++) {
    putchar(c);
    *s++ = c;
  }
  *s = NULL;
  putchar('\n');

  char buffer[1] = {'T'};
  if (write(self->sock, buffer, 1) < 0) {
    pexit("Failed to to write to client");
  }

  while (*self->tshm->shm != '*') {
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
