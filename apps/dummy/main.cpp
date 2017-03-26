#include "client/turkey.h"

int main(int argc, char *argv[]) {
  fprintf(stderr, "CLIENT LOGS\n");
  TURKEY *client = turkey_init();

  // unsigned char *s;
  // for (s = client->tshm->shm; *s != NULL; s++) {
  //   putchar(*s);
  // }
  // putchar('\n');
  //
  // *client->tshm->shm = '*';


  turkey_destroy(client);
  fprintf(stderr, "Finishing\n");
}
