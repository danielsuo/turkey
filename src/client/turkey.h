#ifndef TURKEY_CLIENT_H
#define TURKEY_CLIENT_H

#include <stdio.h>
#include <error.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "common.h"
#include "general_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

struct turkey {
  int pid;
  char *server_ip;
  int sock;

  struct sockaddr_in serv_addr;
  struct turkey_shm *tshm;

  // int pid;
  // size_t shm_key_path_len;
  // char *shm_key_path;
  // key_t shm_key;
  // int shm_id;
  //
  // unsigned char* shm;
};

typedef struct turkey TURKEY;

TURKEY *turkey_init();
void turkey_destroy(TURKEY *turkey_client);

#ifdef __cplusplus
}
#endif

#endif // TURKEY_CLIENT_H
