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

  void *buffer;
  size_t size;
  flatcc_builder_t builder, *B;
  B = &builder;
  flatcc_builder_init(B);

  fprintf(stderr, "Creating root\n");

  Turkey_turkey_msg_register_client_create_as_root(B, client->pid);

  fprintf(stderr, "Building buffer\n");

  buffer = flatcc_builder_finalize_aligned_buffer(B, &size);

  client->req = zsock_new_req(client->server_ip);

  fprintf(stderr, "Trying to create to %s\n", client->server_ip);

  zmsg_t *msg = zmsg_new();
  fprintf(stderr, "Trying to frame to %s\n", client->server_ip);

  zframe_t *frame = zframe_new(buffer, size);
  fprintf(stderr, "Trying to append to %s\n", client->server_ip);

  zmsg_append(msg, &frame);

  fprintf(stderr, "Trying to connect to %s\n", client->server_ip);
  zmsg_send(&msg, client->req);
  fprintf(stderr, "Trying to send to %s\n", client->server_ip);

  zmsg_recv(client->req);

  flatcc_builder_clear(B);

  fprintf(stderr, "All systems go!\n");

  return client;
}

// TODO: we should call this on failure too
void turkey_destroy(TURKEY *client) {
  // close(client->sock);
  zsock_destroy(&client->req);
  turkey_shm_destroy(client->tshm);
  free(client);
}
