#ifndef TURKEY_COMMON_MESSAGES_H
#define TURKEY_COMMON_MESSAGES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

// For TCP sockets.
#define TURKEY_SERVER_IP_KEY "TURKEY_SERVER_IP_KEY"
#define TURKEY_SERVER_PORT 21217

// For shared memory
#define TURKEY_SHM_PATH_FORMAT "/dev/shm/turkey-%08d"
#define TURKEY_SHM_PATH "/dev/shm"
#define TURKEY_SHM_SIZE 1024

// For signalling. Not currently used.
#define TURKEY_SERVER_PID_KEY "TURKEY_SERVER_PID_KEY"

// For UNIX sockets. Not currently used.
#define TURKEY_SERVER_PATH "/var/run/turkey.sock"

#ifdef __cplusplus
extern "C" {
#endif

struct turkey_shm {
  int pid;

  size_t shm_key_path_len;
  char *shm_key_path;
  char *backup;
  key_t shm_key;
  int shm_id;

  unsigned char* shm;
};

struct turkey_shm *turkey_shm_init(int pid);
void turkey_shm_destroy(struct turkey_shm *tshm);

#ifdef __cplusplus
}
#endif

#endif // TURKEY_COMMON_MESSAGES_H
