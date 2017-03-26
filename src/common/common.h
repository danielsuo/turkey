#ifndef TURKEY_COMMON_MESSAGES_H
#define TURKEY_COMMON_MESSAGES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

#include "fbs/fbs.h"
#include "utils/general.h"

// For TCP sockets.
#define TURKEY_SERVER_IP_KEY "TURKEY_SERVER_IP_KEY"
#define TURKEY_SERVER_PORT 21218

// For shared memory
#define TURKEY_SHM_PATH_FORMAT "/dev/shm/turkey-%08d"
#define TURKEY_SHM_PATH "/dev/shm"
#define TURKEY_SHM_SIZE 1024

// For zmq inproc
#define TURKEY_INPROC_FORMAT "turkey-%08d"

// For signalling. Not currently used.
#define TURKEY_SERVER_PID_KEY "TURKEY_SERVER_PID_KEY"

// For UNIX sockets. Not currently used.
#define TURKEY_SERVER_PATH "/var/run/turkey.sock"

#ifdef __cplusplus
extern "C" {
#endif

struct turkey_shm {
  pid_t pid;

  size_t shm_key_path_len;
  char *shm_key_path;
  char *backup;
  key_t shm_key;
  int shm_id;

  unsigned char* shm;

  struct turkey_data *data;
};

struct turkey_data {
  pid_t cpid;
  pid_t spid;

  int32_t cpu_shares;
};

struct turkey_shm *turkey_shm_init(pid_t pid);
void turkey_shm_destroy(struct turkey_shm *tshm);
int turkey_shm_read(struct turkey_shm *tshm, void *buffer, size_t size);
int turkey_shm_write(struct turkey_shm *tshm, void *buffer, size_t size);
int turkey_shm_lock(struct turkey_shm *tshm);
int turkey_shm_unlock(struct turkey_shm *tshm);

int turkey_data_read(struct turkey_shm *tshm);
int turkey_data_write(struct turkey_shm *tshm);

struct turkey_data *turkey_data_init();
void turkey_data_destroy(struct turkey_data *tshm);

#ifdef __cplusplus
}
#endif

#endif // TURKEY_COMMON_MESSAGES_H
