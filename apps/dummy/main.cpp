#define _GNU_SOURCE
#include <linux/sched.h>

#include "client/turkey.h"

int main(int argc, char *argv[]) {
  fprintf(stderr, "CLIENT LOGS\n");
  // TURKEY *client = turkey_init();
  //
  // turkey_destroy(client);

  fprintf(stderr, "CLONE_NEWCGROUP %d\n", CLONE_NEWCGROUP);

  fprintf(stderr, "Finishing\n");
}
