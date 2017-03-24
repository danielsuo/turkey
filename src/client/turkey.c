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

  fprintf(stderr, "Received server ip at %s\n", client->server_ip);


  // flatcc_builder_t builder;
  // flatcc_builder_t *B = &builder;
  // flatcc_builder_init(B);
  //
  // // NOTE: builder documentation https://github.com/dvidelabs/flatcc/blob/master/doc/builder.md
  // // ns(turkey_msg_register_client_t) msg = { client->pid };
  // ns(turkey_msg_register_client_create_as_root)(B, client->pid);
  //
  // void *buf;
  // size_t size;
  // buf = flatcc_builder_finalize_buffer(B, &size);

  void *buffer;
  size_t size;
  flatcc_builder_t builder, *B;
  B = &builder;
  flatcc_builder_init(B);
  Turkey_turkey_msg_register_client_ref_t tmsg;

  fprintf(stderr, "Creating root\n");

  Turkey_turkey_msg_register_client_create_as_root(B, client->pid);

  fprintf(stderr, "Building buffer\n");

  buffer = flatcc_builder_finalize_aligned_buffer(B, &size);

  // int addr_len;
  // if ((addr_len = snprintf(NULL, 0, "tcp://%s:%d", client->server_ip, TURKEY_SERVER_PORT)) < 0) {
  //   pexit("Failed to get server address string length");
  // }
  //
  // char *addr;
  // if ((addr = (char *)malloc(++addr_len)) == NULL) {
  //   pexit("Failed to allocate memory for server address string");
  // }
  //
  // if (snprintf(addr, addr_len, "tcp://%s:%d", client->server_ip, 21218) < 0) {
  //   pexit("Failed to create server address string");
  // }

  client->req = zsock_new_req(client->server_ip);
  // zstr_send(client->req, "Hello, world!");

  // TODO: https://github.com/zeromq/cppzmq/blob/master/zmq.hpp

  // zmq_errno()
  // zmq_strerror (errnum);
  fprintf(stderr, "Trying to create to %s\n", client->server_ip);

  zmsg_t *msg = zmsg_new();
  fprintf(stderr, "Trying to frame to %s\n", client->server_ip);

  zframe_t *frame = zframe_new(buffer, size);
  fprintf(stderr, "Trying to append to %s\n", client->server_ip);

  zmsg_append(msg, &frame);

  fprintf(stderr, "Trying to connect to %s\n", client->server_ip);
  zmsg_send(&msg, client->req);
  fprintf(stderr, "Trying to send to %s\n", client->server_ip);

  // zmsg_destroy(&msg);
  // free(addr);
  // free(buffer);
  flatcc_builder_clear(B);

  // if ((client->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
  //   pexit("Failed to open socket");
  // }
  //
  // bzero((char *) &client->serv_addr, sizeof(client->serv_addr));
  // client->serv_addr.sin_family = AF_INET;
  // inet_aton(client->server_ip, &client->serv_addr.sin_addr);
  // client->serv_addr.sin_port = htons(TURKEY_SERVER_PORT);
  //
  // if (connect(client->sock, (struct sockaddr *) &client->serv_addr, sizeof(client->serv_addr)) < 0) {
  //   pexit("Failed to connect to server");
  // }
  //
  // fprintf(stderr, "Connected to server %s\n", client->server_ip);
  //
  // // Convert pid to network order
  // uint32_t pid = (uint32_t)client->pid;
  // uint32_t n_pid = htonl(pid);
  //
  // if (write(client->sock, &n_pid, sizeof(n_pid)) < 0) {
  //   pexit("Failed to to send pid to server");
  // }
  //
  // int n;
  // char buffer2[1];
  // if ((n = read(client->sock, buffer2, 1)) < 0) {
  //   pexit("Failed to read data from server");
  // }

  fprintf(stderr, "All systems go!\n");

  // if (kill(client->server_pid, SIGINT) < 0) {
  //   pexit("Failed to register with Turkey server");
  // }

  return client;
}

// TODO: we should call this on failure too
void turkey_destroy(TURKEY *client) {
  // close(client->sock);
  zsock_destroy(&client->req);
  turkey_shm_destroy(client->tshm);
  free(client);
}
