#pragma once

#include "list.h"

// NOTE: Graph structures
// - Many to one
// - One to many
// - Many to many
// - Shared output / input

// NOTE:
// - Fixed data
// - Fixed code
// - How do iterations happen

// TODO
// - Need queue: https://github.com/cgaebel/pipe
// - Need some way of indicating level of parallelism we'd like
// - Think about work queue vs dividing up in the beginning; maybe we can get away with the former in non-distributed setting
// - Wait for all tasks

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _turkey_pool {
  // NOTE: assume all children must process output
  list *children;

  size_t num_threads;
  size_t target_num_threads;
} turkey_pool;

turkey_pool *turkey_pool_init();

// One-off thread
int turkey_pool_add_task(turkey_pool *pool, void (*func)(void *), void *arg);
int turkey_pool_start_tasks(turkey_pool *pool);

// Where we can turn parallelism up and down. Can increase number of threads at any time
int turkey_pool_process_queue(turkey_pool *pool);
int turkey_pool_thread_target(turkey_pool *pool, size_t target);

#ifdef __cplusplus
}
#endif
