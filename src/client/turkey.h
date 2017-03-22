#ifndef TURKEY_CLIENT_H
#define TURKEY_CLIENT_H

#include <stdio.h>
#include <error.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"{
#endif

#define TURKEY_SERVER_PID_KEY "TURKEY_SERVER_PID_KEY"

enum TURKEY_CODE {
  TURKEY_SUCCESS
};

struct turkey {
  int pid;
  int server_pid;

  size_t shm_key_path_len;
  char *shm_key_path;
  key_t shm_key;
};

typedef struct turkey TURKEY;

TURKEY *turkey_init();
void turkey_destroy(TURKEY *turkey_client);

#ifdef __cplusplus
}
#endif

#endif // TURKEY_CLIENT_H
