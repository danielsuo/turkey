#pragma once

#include <stdio.h>
#include <error.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "common/common.h"
#include "fbs/fbs.h"
#include "utils/general.h"

#include "block.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _turkey {
  turkey_shm *tshm;
} turkey;

turkey *turkey_init();
void turkey_destroy(turkey *turkey_client);

#ifdef __cplusplus
}
#endif
