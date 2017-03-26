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
  client->tcpu = turkey_cpu_init();
  client->tshm = turkey_shm_init(client->pid);

  if (signal(SIGINT, handler) == SIG_ERR) {
    pexit("Failed to handle SIGINT signal");
  }

  if ((client->server_ip = getenv(TURKEY_SERVER_IP_KEY)) == NULL) {
    pexit("Failed to get Turkey server ip");
  }

  fprintf(stderr, "Received server ip at %s\n", client->server_ip);

  // Initializing buffer
  void *buffer;
  size_t size;
  flatcc_builder_t builder, *B;
  B = &builder;
  flatcc_builder_init(B);

  // Building buffer
  Turkey_turkey_msg_register_client_create_as_root(B, client->pid);
  buffer = flatcc_builder_finalize_aligned_buffer(B, &size);

  // Open REQ socket (send, receive pattern)
  client->req = zsock_new_req(client->server_ip);

  // Build message
  zmsg_t *msg = zmsg_new();
  zframe_t *frame = zframe_new(buffer, size);
  zmsg_append(msg, &frame);

  // Send message
  zmsg_send(&msg, client->req);

  // Wait for a server response
  zmsg_recv(client->req);

  Turkey_turkey_shm_cpu_table_t table = Turkey_turkey_shm_cpu_as_root(client->tshm->shm);
  client->tcpu->share = Turkey_turkey_shm_cpu_share(table);
  fprintf(stderr, "Got %d share\n", client->tcpu->share);

  flatcc_builder_clear(B);

  fprintf(stderr, "All systems go!\n");

  return client;
}

// TODO: we should call this on failure too
void turkey_destroy(TURKEY *client) {
  zsock_destroy(&client->req);
  turkey_cpu_destroy(client->tcpu);
  turkey_shm_destroy(client->tshm);
  free(client);
}
