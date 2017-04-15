
#ifndef __TASKQ_DISTLIST_H__
#define __TASKQ_DISTLIST_H__

#define VERSION "Fixed Arrays"

#include "taskQList.h"

typedef struct {
  volatile TaskQList shared;
  char padding[CACHE_LINE_SIZE];
} TaskQDetails;

typedef struct {
#ifdef ENABLE_PTHREADS
  pthread_mutex_t lock;
#endif // ENABLE_PTHREADS
  long statEnqueued, statLocal, *statStolen;
  char padding1[CACHE_LINE_SIZE];
  TaskQDetails q;
  char padding2[CACHE_LINE_SIZE];
} TaskQ;

#endif
