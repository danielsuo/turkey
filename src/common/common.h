#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

#include "fbs/fbs.h"
#include "utils/general.h"

#define TURKEY_FORMAT "turkey-%08d"

// For TCP sockets.
#define TURKEY_SERVER_IP_KEY "TURKEY_SERVER_IP_KEY"
#define TURKEY_SERVER_PORT 21218

// For shared memory
#define TURKEY_SHM_PATH "/dev/shm"
#define TURKEY_SHM_PATH_FORMAT TURKEY_SHM_PATH TURKEY_FORMAT
#define TURKEY_SHM_SIZE 1024

// For signalling. Not currently used.
#define TURKEY_SERVER_PID_KEY "TURKEY_SERVER_PID_KEY"

// For UNIX sockets. Not currently used.
#define TURKEY_SERVER_PATH "/var/run/turkey.sock"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _turkey_data {
  pid_t cpid;
  pid_t spid;

  int32_t cpu_shares;
} turkey_data;

typedef struct _turkey_shm {
  pid_t pid;

  size_t shm_key_path_len;
  char *shm_key_path;
  char *backup;
  key_t shm_key;
  int shm_id;

  unsigned char* shm;

  turkey_data *data;
} turkey_shm;

turkey_shm *turkey_shm_init(pid_t pid);
void turkey_shm_destroy(turkey_shm *tshm);
int turkey_shm_read(turkey_shm *tshm, void *buffer, size_t size);
int turkey_shm_write(turkey_shm *tshm, void *buffer, size_t size);
int turkey_shm_lock(turkey_shm *tshm);
int turkey_shm_unlock(turkey_shm *tshm);

int turkey_data_read(turkey_shm *tshm);
int turkey_data_write(turkey_shm *tshm);

turkey_data *turkey_data_init();
void turkey_data_destroy(turkey_data *tshm);

#ifdef __cplusplus
}
#endif
