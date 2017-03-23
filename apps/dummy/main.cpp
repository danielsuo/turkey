#include "turkey.h"

int main(int argc, char *argv[]) {
  TURKEY *client = turkey_init();

  fprintf(stderr, "Starting\n");

  unsigned char *s;
  for (s = client->tshm->shm; *s != NULL; s++) {
    putchar(*s);
  }
  putchar('\n');

  *client->tshm->shm = '*';

  fprintf(stderr, "Finishing\n");

  turkey_destroy(client);
}
