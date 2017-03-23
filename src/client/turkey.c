#include "turkey.h"

void handler(int signum) { printf("Received signal %d\n", signum); }

// TODO: write data to shared memory to indicate state (e.g., start / stop)
// TODO: send signal in turkey_destroy to say job completed
TURKEY *turkey_init() {
  TURKEY *client;

  if ((client = (TURKEY *)malloc(sizeof(TURKEY))) == NULL) {
    pexit("Failed to allocate memory for Turkey client");
  }

  // TODO: this is 1 unless we use an init process or run in priveleged mode
  //       (which is what we're assuming...)
  client->pid = getpid();

  client->tshm = turkey_shm_init(client->pid);

  if (signal(SIGINT, handler) == SIG_ERR) {
    pexit("Failed to handle SIGINT signal");
  }

  if ((client->server_ip = getenv(TURKEY_SERVER_IP_KEY)) == NULL) {
    pexit("Failed to get Turkey server ip");
  }

  if ((client->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    pexit("Failed to open socket");
  }

  bzero((char *) &client->serv_addr, sizeof(client->serv_addr));
  client->serv_addr.sin_family = AF_INET;
  inet_aton(client->server_ip, &client->serv_addr.sin_addr);
  client->serv_addr.sin_port = htons(TURKEY_SERVER_PORT);

  if (connect(client->sock, (struct sockaddr *) &client->serv_addr, sizeof(client->serv_addr)) < 0) {
    pexit("Failed to connect to server");
  }

  fprintf(stderr, "Connected to server %s\n", client->server_ip);

  // Convert pid to network order
  uint32_t pid = (uint32_t)client->pid;
  uint32_t n_pid = htonl(pid);

  if (write(client->sock, &n_pid, sizeof(n_pid)) < 0) {
    pexit("Failed to to send pid to server");
  }

  int n;
  char buffer[1];
  if ((n = read(client->sock, buffer, 1)) < 0) {
    pexit("Failed to read data from server");
  }

  fprintf(stderr, "All systems go!\n");

  // if (kill(client->server_pid, SIGINT) < 0) {
  //   pexit("Failed to register with Turkey server");
  // }

  return client;
}

// TODO: we should call this on failure too
void turkey_destroy(TURKEY *client) {
  close(client->sock);
  turkey_shm_destroy(client->tshm);
  free(client);
}
