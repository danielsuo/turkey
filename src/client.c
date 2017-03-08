#include "client.h"

void handler(int signum) {
  printf("Received signal %d\n", signum);
}

void pexit(char *msg) {
  perror(msg);
  exit(1);
}

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

  char *server_pid;
  if ((server_pid = getenv(TURKEY_SERVER_PID_KEY)) == NULL) {
    pexit("Failed to get Turkey server pid");
  }

  if ((client->server_pid = atoi(server_pid)) == 0) {
    pexit("Failed to parse Turkey server pid");
  }

  if ((client->shm_key_path_len = snprintf(NULL, 0, "/dev/shm/turkey-%08d", client->pid)) < 0) {
    pexit("Failed to get shared memory path length");
  }

  if ((client->shm_key_path = (char *)malloc(++client->shm_key_path_len)) == NULL) {
    pexit("Failed to allocate memory for shared memory file name");
  }

  if (snprintf(client->shm_key_path, client->shm_key_path_len, "/dev/shm/turkey-%08d", client->pid) < 0) {
    pexit("Failed to create shared memory file name string");
  }

  printf("%d: %d -> %s\n", client->server_pid, client->pid, client->shm_key_path);

  FILE *shm_file;
  if (!(shm_file = fopen(client->shm_key_path, "wt"))) {
    pexit("Failed to create file for shared memory");
  }
  fclose(shm_file);

  if (client->shm_key = ftok(client->shm_key_path, 1) == -1) {
    pexit("Failed to get IPC key from file name");
  }

  if (signal(SIGINT, handler) == SIG_ERR) {
    pexit("Failed to handle SIGINT signal");
  }

  if (kill(client->server_pid, SIGINT) < 0) {
    pexit("Failed to register with Turkey server");
  }

  return client;
}

void turkey_destroy(TURKEY *turkey_client) {
  if (remove(turkey_client->shm_key_path) < 0) {
    pexit("Failed to remove shared memory file");
  }

  free(turkey_client->shm_key_path);
  free(turkey_client);
}
