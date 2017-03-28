#include "turkey.h"

// TODO: write data to shared memory to indicate state (e.g., start / stop)
// TODO: send signal in turkey_destroy to say job completed
turkey *turkey_init() {

  turkey *client;

  if ((client = (turkey *)malloc(sizeof(turkey))) == NULL) {
    pexit("Failed to allocate memory for Turkey client");
  }

  client->tshm = turkey_shm_init(getpid());

  turkey_data_read(client->tshm);
  fprintf(stderr, "Got %d share from %d to %d (%d)\n",
          client->tshm->data->cpu_shares,
          client->tshm->data->spid,
          client->tshm->data->cpid,
          getpid()
        );

  return client;
}

// TODO: we should call this on failure too
void turkey_destroy(turkey *client) {
  turkey_shm_destroy(client->tshm);
  free(client);
}
