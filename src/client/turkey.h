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

#include <czmq.h>

#include "common/common.h"
#include "fbs/fbs.h"
#include "utils/general.h"

#ifdef __cplusplus
extern "C" {
#endif

struct turkey {
//   pid_t cpid; // client pid
//   pid_t spid; // server pid

  struct turkey_shm *tshm;
};

typedef struct turkey TURKEY;

TURKEY *turkey_init();
void turkey_destroy(TURKEY *turkey_client);

#ifdef __cplusplus
}
#endif

#endif // TURKEY_CLIENT_H
