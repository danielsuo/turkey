#include "turkey.h"

int main(int argc, char *argv[]) {
  fprintf(stderr, "CLIENT LOGS\n");
  TURKEY *client = turkey_init();

  unsigned char *s;
  for (s = client->tshm->shm; *s != NULL; s++) {
    putchar(*s);
  }
  putchar('\n');

  *client->tshm->shm = '*';

  fprintf(stderr, "Finishing\n");

  turkey_destroy(client);
}
