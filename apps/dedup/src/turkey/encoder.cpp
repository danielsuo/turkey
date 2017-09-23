/*
 * Decoder for dedup files
 *
 * Copyright 2010 Princeton University.
 * All rights reserved.
 *
 * Originally written by Minlan Yu.
 * Largely rewritten by Christian Bienia.
 */

/*
 * The pipeline model for Encode is
 * Fragment->FragmentRefine->Deduplicate->Compress->Reorder
 * Each stage has basically three steps:
 * 1. fetch a group of items from the queue
 * 2. process the items
 * 3. put them in the queue for the next stage
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

extern "C" {
#include "config.h"
#include "debug.h"
#include "dedupdef.h"
#include "encoder.h"
#include "hashtable.h"
#include "mbuffer.h"
#include "rabin.h"
#include "util.h"

#include "binheap.h"
#include "queue.h"
#include "tree.h"
}

#include <folly/MPMCQueue.h>
#include <glog/logging.h>
#include "Client.h"

#ifdef ENABLE_GZIP_COMPRESSION
#include <zlib.h>
#endif // ENABLE_GZIP_COMPRESSION

#ifdef ENABLE_BZIP2_COMPRESSION
#include <bzlib.h>
#endif // ENABLE_BZIP2_COMPRESSION

#include <pthread.h>

#define INITIAL_SEARCH_TREE_SIZE 4096

// The queues between the pipeline stages
folly::MPMCQueue<void*> queue_refine(QUEUE_SIZE);
folly::MPMCQueue<void*> queue_deduplicate(QUEUE_SIZE);
folly::MPMCQueue<void*> queue_compress(QUEUE_SIZE);
folly::MPMCQueue<void*> queue_reorder(QUEUE_SIZE);

int enqueue_from_ringbuffer(folly::MPMCQueue<void*>& queue, ringbuffer_t* buf,
                            int num) {
  int i;

  // Try to enqueue up to num elements
  for (i = 0; i < num && !ringbuffer_isEmpty(buf); i++) {

    // Peek at the ringbuffer tail and attempt to enqueue
    void* tmp = ringbuffer_peek(buf);
    assert(tmp != NULL);

    // If write succeeds, remove from ringbuffer
    if (queue.write(tmp)) {
      ringbuffer_remove(buf);
    }
    // Otherwise, keep in ring buffer and break
    else {
      break;
    }
  }

  return i;
}

int dequeue_to_ringbuffer(folly::MPMCQueue<void*>& queue, ringbuffer_t* buf,
                          int num) {
  int i;

  for (i = 0; i < num && !ringbuffer_isFull(buf); i++) {
    void* tmp;

    if (queue.read(tmp)) {
      if (tmp == nullptr) {
        // Enqueue two more sentinel nullptrs so we shutdown in O(log C) instead
        // of O(C)
        // TODO: check return values?
        queue.write(nullptr);
        queue.write(nullptr);
        return -1;
      }
      assert(tmp != NULL);
      int r = ringbuffer_insert(buf, tmp);
      assert(r >= 0);
    } else {
      break;
    }
  }

  return i;
}

void terminate_queue(folly::MPMCQueue<void*>& queue) {
  // TODO: do we want to drain the queue as well?
  // TODO: verify there's room to write the null pointer
  // TODO: check return value
  queue.write(nullptr);
}

// The configuration block defined in main
extern config_t* conf;

// Hash table data structure & utility functions
struct hashtable* cache;

static unsigned int hash_from_key_fn(void* k) {
  // NOTE: sha1 sum is integer-aligned
  return ((unsigned int*)k)[0];
}

static int keys_equal_fn(void* key1, void* key2) {
  return (memcmp(key1, key2, SHA1_LEN) == 0);
}

// Arguments to pass to each thread
struct thread_args {
  // thread id, unique within a thread pool (i.e. unique for a pipeline stage)
  int tid;
  // file descriptor, first pipeline stage only
  int fd;
  // input file buffer, first pipeline stage & preloading only
  struct {
    void* buffer;
    size_t size;
  } input_file;
};

// Keep track of block granularity with 2^CHUNK_GRANULARITY_POW resolution (for
// statistics)
#define CHUNK_GRANULARITY_POW (7)
// Number of blocks to distinguish, CHUNK_MAX_NUM * 2^CHUNK_GRANULARITY_POW is
// biggest block being recognized (for statistics)
#define CHUNK_MAX_NUM (8 * 32)
// Map a chunk size to a statistics array slot
#define CHUNK_SIZE_TO_SLOT(s)                                                  \
  (((s) >> (CHUNK_GRANULARITY_POW)) >= (CHUNK_MAX_NUM)                         \
       ? (CHUNK_MAX_NUM)-1                                                     \
       : ((s) >> (CHUNK_GRANULARITY_POW)))
// Get the average size of a chunk from a statistics array slot
#define SLOT_TO_CHUNK_SIZE(s)                                                  \
  ((s) * (1 << (CHUNK_GRANULARITY_POW)) + (1 << ((CHUNK_GRANULARITY_POW)-1)))
// Deduplication statistics (only used if ENABLE_STATISTICS is defined)
typedef struct {
  /* Cumulative sizes */
  size_t total_input; // Total size of input in bytes
  size_t total_dedup; // Total size of input without duplicate blocks (after
                      // global compression) in bytes
  size_t total_compressed; // Total size of input stream after local compression
                           // in bytes
  size_t total_output; // Total size of output in bytes (with overhead) in bytes

  /* Size distribution & other properties */
  unsigned int nChunks[CHUNK_MAX_NUM]; // Coarse-granular size distribution of
                                       // data chunks
  unsigned int nDuplicates;            // Total number of duplicate blocks
} stats_t;

// Simple write utility function
static int write_file(int fd, u_char type, u_long len, u_char* content) {
  if (xwrite(fd, &type, sizeof(type)) < 0) {
    perror("xwrite:");
    EXIT_TRACE("xwrite type fails\n");
    return -1;
  }
  if (xwrite(fd, &len, sizeof(len)) < 0) {
    EXIT_TRACE("xwrite content fails\n");
  }
  if (xwrite(fd, content, len) < 0) {
    EXIT_TRACE("xwrite content fails\n");
  }
  return 0;
}

/*
 * Helper function that creates and initializes the output file
 * Takes the file name to use as input and returns the file handle
 * The output file can be used to write chunks without any further steps
 */
static int create_output_file(char* outfile) {
  int fd;

  // Create output file
  fd = open(outfile, O_CREAT | O_TRUNC | O_WRONLY | O_TRUNC,
            S_IRGRP | S_IWUSR | S_IRUSR | S_IROTH);
  if (fd < 0) {
    EXIT_TRACE("Cannot open output file.");
  }

  // Write header
  if (write_header(fd, conf->compress_type)) {
    EXIT_TRACE("Cannot write output file header.\n");
  }

  return fd;
}

/*
 * Helper function that writes a chunk to an output file depending on
 * its state. The function will write the SHA1 sum if the chunk has
 * already been written before, or it will write the compressed data
 * of the chunk if it has not been written yet.
 *
 * This function will block if the compressed data is not available yet.
 * This function might update the state of the chunk if there are any changes.
 */
// NOTE: The parallel version checks the state of each chunk to make sure the
//      relevant data is available. If it is not then the function waits.
static void write_chunk_to_file(int fd, chunk_t* chunk) {
  assert(chunk != NULL);

  // Find original chunk
  if (chunk->header.isDuplicate)
    chunk = chunk->compressed_data_ref;

  pthread_mutex_lock(&chunk->header.lock);
  while (chunk->header.state == CHUNK_STATE_UNCOMPRESSED) {
    pthread_cond_wait(&chunk->header.update, &chunk->header.lock);
  }

  // state is now guaranteed to be either COMPRESSED or FLUSHED
  if (chunk->header.state == CHUNK_STATE_COMPRESSED) {
    // Chunk data has not been written yet, do so now
    write_file(fd, TYPE_COMPRESS, chunk->compressed_data.n,
               chunk->compressed_data.ptr);
    mbuffer_free(&chunk->compressed_data);
    chunk->header.state = CHUNK_STATE_FLUSHED;
  } else {
    // Chunk data has been written to file before, just write SHA1
    write_file(fd, TYPE_FINGERPRINT, SHA1_LEN, (unsigned char*)(chunk->sha1));
  }
  pthread_mutex_unlock(&chunk->header.lock);
}

int rf_win;
int rf_win_dataprocess;

/*
 * Computational kernel of compression stage
 *
 * Actions performed:
 *  - Compress a data chunk
 */
void sub_Compress(chunk_t* chunk) {
  size_t n;
  int r;

  assert(chunk != NULL);
  // compress the item and add it to the database
  pthread_mutex_lock(&chunk->header.lock);
  assert(chunk->header.state == CHUNK_STATE_UNCOMPRESSED);
  switch (conf->compress_type) {
  case COMPRESS_NONE:
    // Simply duplicate the data
    n = chunk->uncompressed_data.n;
    r = mbuffer_create(&chunk->compressed_data, n);
    if (r != 0) {
      EXIT_TRACE("Creation of compression buffer failed.\n");
    }
    // copy the block
    memcpy(chunk->compressed_data.ptr, chunk->uncompressed_data.ptr,
           chunk->uncompressed_data.n);
    break;
#ifdef ENABLE_GZIP_COMPRESSION
  case COMPRESS_GZIP:
    // Gzip compression buffer must be at least 0.1% larger than source buffer
    // plus 12 bytes
    n = chunk->uncompressed_data.n + (chunk->uncompressed_data.n >> 9) + 12;
    r = mbuffer_create(&chunk->compressed_data, n);
    if (r != 0) {
      EXIT_TRACE("Creation of compression buffer failed.\n");
    }
    // compress the block
    r = compress(chunk->compressed_data.ptr, &n, chunk->uncompressed_data.ptr,
                 chunk->uncompressed_data.n);
    if (r != Z_OK) {
      EXIT_TRACE("Compression failed\n");
    }
    // Shrink buffer to actual size
    if (n < chunk->compressed_data.n) {
      r = mbuffer_realloc(&chunk->compressed_data, n);
      assert(r == 0);
    }
    break;
#endif // ENABLE_GZIP_COMPRESSION
#ifdef ENABLE_BZIP2_COMPRESSION
  case COMPRESS_BZIP2:
    // Bzip compression buffer must be at least 1% larger than source buffer
    // plus 600 bytes
    n = chunk->uncompressed_data.n + (chunk->uncompressed_data.n >> 6) + 600;
    r = mbuffer_create(&chunk->compressed_data, n);
    if (r != 0) {
      EXIT_TRACE("Creation of compression buffer failed.\n");
    }
    // compress the block
    unsigned int int_n = n;
    r = BZ2_bzBuffToBuffCompress(chunk->compressed_data.ptr, &int_n,
                                 chunk->uncompressed_data.ptr,
                                 chunk->uncompressed_data.n, 9, 0, 30);
    n = int_n;
    if (r != BZ_OK) {
      EXIT_TRACE("Compression failed\n");
    }
    // Shrink buffer to actual size
    if (n < chunk->compressed_data.n) {
      r = mbuffer_realloc(&chunk->compressed_data, n);
      assert(r == 0);
    }
    break;
#endif // ENABLE_BZIP2_COMPRESSION
  default:
    EXIT_TRACE("Compression type not implemented.\n");
    break;
  }
  mbuffer_free(&chunk->uncompressed_data);

  chunk->header.state = CHUNK_STATE_COMPRESSED;
  pthread_cond_broadcast(&chunk->header.update);
  pthread_mutex_unlock(&chunk->header.lock);

  return;
}

/*
 * Pipeline stage function of compression stage
 *
 * Actions performed:
 *  - Dequeue items from compression queue
 *  - Execute compression kernel for each item
 *  - Enqueue each item into send queue
 */
void* Compress(void* targs) {
  struct thread_args* args = (struct thread_args*)targs;
  chunk_t* chunk;
  int r;

  ringbuffer_t recv_buf, send_buf;

  r = 0;
  r += ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
  r += ringbuffer_init(&send_buf, ITEM_PER_INSERT);
  assert(r == 0);

  while (1) {
    // get items from the queue
    if (ringbuffer_isEmpty(&recv_buf)) {
      // r = queue_dequeue(compress_que, &recv_buf, ITEM_PER_FETCH);
      r = dequeue_to_ringbuffer(queue_compress, &recv_buf, ITEM_PER_FETCH);
      if (r < 0)
        break;
    }

    // fetch one item
    chunk = (chunk_t*)ringbuffer_remove(&recv_buf);
    // assert(chunk != NULL);
    if (chunk == NULL) {
      continue;
    }

    sub_Compress(chunk);

    r = ringbuffer_insert(&send_buf, chunk);
    assert(r == 0);

    // put the item in the next queue for the write thread
    if (ringbuffer_isFull(&send_buf)) {
      // r = queue_enqueue(reorder_que, &send_buf, ITEM_PER_INSERT);
      r = enqueue_from_ringbuffer(queue_reorder, &send_buf, ITEM_PER_INSERT);
      assert(r >= 1);
    }
  }

  // Enqueue left over items
  while (!ringbuffer_isEmpty(&send_buf)) {
    // r = queue_enqueue(reorder_que, &send_buf, ITEM_PER_INSERT);
    r = enqueue_from_ringbuffer(queue_reorder, &send_buf, ITEM_PER_INSERT);
    assert(r >= 1);
  }

  ringbuffer_destroy(&recv_buf);
  ringbuffer_destroy(&send_buf);

  // shutdown
  // queue_terminate(reorder_que);
  terminate_queue(queue_reorder);

  return NULL;
}

/*
 * Computational kernel of deduplication stage
 *
 * Actions performed:
 *  - Calculate SHA1 signature for each incoming data chunk
 *  - Perform database lookup to determine chunk redundancy status
 *  - On miss add chunk to database
 *  - Returns chunk redundancy status
 */
int sub_Deduplicate(chunk_t* chunk) {
  int isDuplicate;
  chunk_t* entry;

  assert(chunk != NULL);
  assert(chunk->uncompressed_data.ptr != NULL);

  SHA1_Digest(chunk->uncompressed_data.ptr, chunk->uncompressed_data.n,
              (unsigned char*)(chunk->sha1));

  // Query database to determine whether we've seen the data chunk before
  pthread_mutex_t* ht_lock = hashtable_getlock(cache, (void*)(chunk->sha1));
  pthread_mutex_lock(ht_lock);
  entry       = (chunk_t*)hashtable_search(cache, (void*)(chunk->sha1));
  isDuplicate = (entry != NULL);
  chunk->header.isDuplicate = isDuplicate;
  if (!isDuplicate) {
    // Cache miss: Create entry in hash table and forward data to compression
    // stage
    pthread_mutex_init(&chunk->header.lock, NULL);
    pthread_cond_init(&chunk->header.update, NULL);
    // NOTE: chunk->compressed_data.buffer will be computed in compression stage
    if (hashtable_insert(cache, (void*)(chunk->sha1), (void*)chunk) == 0) {
      EXIT_TRACE("hashtable_insert failed");
    }
  } else {
    // Cache hit: Skipping compression stage
    chunk->compressed_data_ref = entry;
    mbuffer_free(&chunk->uncompressed_data);
  }
  pthread_mutex_unlock(ht_lock);

  return isDuplicate;
}

/*
 * Pipeline stage function of deduplication stage
 *
 * Actions performed:
 *  - Take input data from fragmentation stages
 *  - Execute deduplication kernel for each data chunk
 *  - Route resulting package either to compression stage or to reorder stage,
 * depending on deduplication status
 */
void* Deduplicate(void* targs) {
  struct thread_args* args = (struct thread_args*)targs;
  /* const int qid            = args->tid / MAX_THREADS_PER_QUEUE; */
  chunk_t* chunk;
  int r;

  ringbuffer_t recv_buf, send_buf_reorder, send_buf_compress;

  r = 0;
  r += ringbuffer_init(&recv_buf, CHUNK_ANCHOR_PER_FETCH);
  r += ringbuffer_init(&send_buf_reorder, ITEM_PER_INSERT);
  r += ringbuffer_init(&send_buf_compress, ITEM_PER_INSERT);
  assert(r == 0);

  while (1) {
    // if no items available, fetch a group of items from the queue
    if (ringbuffer_isEmpty(&recv_buf)) {
      // r = queue_dequeue(deduplicate_que, &recv_buf, CHUNK_ANCHOR_PER_FETCH);
      r = dequeue_to_ringbuffer(queue_deduplicate, &recv_buf,
                                CHUNK_ANCHOR_PER_FETCH);
      if (r < 0)
        break;
    }

    // get one chunk
    chunk = (chunk_t*)ringbuffer_remove(&recv_buf);
    // assert(chunk != NULL);
    if (chunk == NULL) {
      continue;
    }

    // Do the processing
    int isDuplicate = sub_Deduplicate(chunk);

    // Enqueue chunk either into compression queue or into send queue
    if (!isDuplicate) {
      r = ringbuffer_insert(&send_buf_compress, chunk);
      assert(r == 0);
      if (ringbuffer_isFull(&send_buf_compress)) {
        // r = queue_enqueue(compress_que, &send_buf_compress, ITEM_PER_INSERT);
        r = enqueue_from_ringbuffer(queue_compress, &send_buf_compress,
                                    ITEM_PER_INSERT);
        assert(r >= 1);
      }
    } else {
      r = ringbuffer_insert(&send_buf_reorder, chunk);
      assert(r == 0);
      if (ringbuffer_isFull(&send_buf_reorder)) {
        // r = queue_enqueue(reorder_que, &send_buf_reorder, ITEM_PER_INSERT);
      r = enqueue_from_ringbuffer(queue_reorder, &send_buf_reorder, ITEM_PER_INSERT);
        assert(r >= 1);
      }
    }
  }

  // empty buffers
  while (!ringbuffer_isEmpty(&send_buf_compress)) {
    // r = queue_enqueue(compress_que, &send_buf_compress, ITEM_PER_INSERT);
    r = enqueue_from_ringbuffer(queue_compress, &send_buf_compress,
                                ITEM_PER_INSERT);
    assert(r >= 1);
  }
  while (!ringbuffer_isEmpty(&send_buf_reorder)) {
    // r = queue_enqueue(reorder_que, &send_buf_reorder, ITEM_PER_INSERT);
      r = enqueue_from_ringbuffer(queue_reorder, &send_buf_reorder, ITEM_PER_INSERT);
    assert(r >= 1);
  }

  ringbuffer_destroy(&recv_buf);
  ringbuffer_destroy(&send_buf_compress);
  ringbuffer_destroy(&send_buf_reorder);

  // shutdown
  // queue_terminate(compress_que);
  terminate_queue(queue_compress);

  return NULL;
}

/*
 * Pipeline stage function and computational kernel of refinement stage
 *
 * Actions performed:
 *  - Take coarse chunks from fragmentation stage
 *  - Partition data block into smaller chunks with Rabin rolling fingerprints
 *  - Send resulting data chunks to deduplication stage
 *
 * Notes:
 *  - Allocates mbuffers for fine-granular chunks
 */
void* FragmentRefine(void* targs) {
  struct thread_args* args = (struct thread_args*)targs;
  /* const int qid            = args->tid / MAX_THREADS_PER_QUEUE; */
  ringbuffer_t recv_buf, send_buf;
  int r;

  chunk_t* temp;
  chunk_t* chunk;
  u32int* rabintab    = malloc(256 * sizeof rabintab[0]);
  u32int* rabinwintab = malloc(256 * sizeof rabintab[0]);
  if (rabintab == NULL || rabinwintab == NULL) {
    EXIT_TRACE("Memory allocation failed.\n");
  }

  r = 0;
  r += ringbuffer_init(&recv_buf, MAX_PER_FETCH);
  r += ringbuffer_init(&send_buf, CHUNK_ANCHOR_PER_INSERT);
  assert(r == 0);

  while (TRUE) {
    // if no item for process, get a group of items from the pipeline
    if (ringbuffer_isEmpty(&recv_buf)) {
      // r = queue_dequeue(refine_que, &recv_buf, MAX_PER_FETCH);
      r = dequeue_to_ringbuffer(queue_refine, &recv_buf, MAX_PER_FETCH);
      if (r < 0) {
        break;
      }
    }

    // get one item
    chunk = (chunk_t*)ringbuffer_remove(&recv_buf);
    // assert(chunk != NULL);
    if (chunk == NULL) {
      continue;
    }

    rabininit(rf_win, rabintab, rabinwintab);

    int split;
    sequence_number_t chcount = 0;
    do {
      // Find next anchor with Rabin fingerprint
      int offset =
          rabinseg(chunk->uncompressed_data.ptr, chunk->uncompressed_data.n,
                   rf_win, rabintab, rabinwintab);
      // Can we split the buffer?
      if (offset < chunk->uncompressed_data.n) {
        // Allocate a new chunk and create a new memory buffer
        temp = (chunk_t*)malloc(sizeof(chunk_t));
        if (temp == NULL)
          EXIT_TRACE("Memory allocation failed.\n");
        temp->header.state   = chunk->header.state;
        temp->sequence.l1num = chunk->sequence.l1num;

        // split it into two pieces
        r = mbuffer_split(&chunk->uncompressed_data, &temp->uncompressed_data,
                          offset);
        if (r != 0)
          EXIT_TRACE("Unable to split memory buffer.\n");

        // Set correct state and sequence numbers
        chunk->sequence.l2num = chcount;
        chunk->isLastL2Chunk  = FALSE;
        chcount++;

        // put it into send buffer
        r = ringbuffer_insert(&send_buf, chunk);
        assert(r == 0);
        if (ringbuffer_isFull(&send_buf)) {
          // r = queue_enqueue(deduplicate_que, &send_buf,
          // CHUNK_ANCHOR_PER_INSERT);
          r = enqueue_from_ringbuffer(queue_deduplicate, &send_buf,
                                      CHUNK_ANCHOR_PER_INSERT);
          assert(r >= 1);
        }
        // prepare for next iteration
        chunk = temp;
        split = 1;
      } else {
        // End of buffer reached, don't split but simply enqueue it
        // Set correct state and sequence numbers
        chunk->sequence.l2num = chcount;
        chunk->isLastL2Chunk  = TRUE;
        chcount++;

        // put it into send buffer
        r = ringbuffer_insert(&send_buf, chunk);
        assert(r == 0);
        if (ringbuffer_isFull(&send_buf)) {
          // r = queue_enqueue(deduplicate_que, &send_buf,
          // CHUNK_ANCHOR_PER_INSERT);
          r = enqueue_from_ringbuffer(queue_deduplicate, &send_buf,
                                      CHUNK_ANCHOR_PER_INSERT);
          assert(r >= 1);
        }
        // prepare for next iteration
        chunk = NULL;
        split = 0;
      }
    } while (split);
  }

  // drain buffer
  while (!ringbuffer_isEmpty(&send_buf)) {
    // r = queue_enqueue(deduplicate_que, &send_buf, CHUNK_ANCHOR_PER_INSERT);
    r = enqueue_from_ringbuffer(queue_deduplicate, &send_buf,
                                CHUNK_ANCHOR_PER_INSERT);
    assert(r >= 1);
  }

  free(rabintab);
  free(rabinwintab);
  ringbuffer_destroy(&recv_buf);
  ringbuffer_destroy(&send_buf);

  // shutdown
  // queue_terminate(deduplicate_que);
  terminate_queue(queue_deduplicate);
  return NULL;
}

/*
 * Integrate all computationally intensive pipeline
 * stages to improve cache efficiency.
 */
void* SerialIntegratedPipeline(void* targs) {
  struct thread_args* args      = (struct thread_args*)targs;
  size_t preloading_buffer_seek = 0;
  int fd                        = args->fd;
  int fd_out                    = create_output_file(conf->outfile);
  int r;

  chunk_t* temp       = NULL;
  chunk_t* chunk      = NULL;
  u32int* rabintab    = malloc(256 * sizeof rabintab[0]);
  u32int* rabinwintab = malloc(256 * sizeof rabintab[0]);
  if (rabintab == NULL || rabinwintab == NULL) {
    EXIT_TRACE("Memory allocation failed.\n");
  }

  rf_win_dataprocess = 0;
  rabininit(rf_win_dataprocess, rabintab, rabinwintab);

  // Sanity check
  if (MAXBUF < 8 * ANCHOR_JUMP) {
    printf("WARNING: I/O buffer size is very small. Performance degraded.\n");
    fflush(NULL);
  }

  // read from input file / buffer
  while (1) {
    size_t bytes_left; // amount of data left over in last_mbuffer from previous
                       // iteration

    // Check how much data left over from previous iteration resp. create an
    // initial chunk
    if (temp != NULL) {
      bytes_left = temp->uncompressed_data.n;
    } else {
      bytes_left = 0;
    }

    // Make sure that system supports new buffer size
    if (MAXBUF + bytes_left > SSIZE_MAX) {
      EXIT_TRACE("Input buffer size exceeds system maximum.\n");
    }
    // Allocate a new chunk and create a new memory buffer
    chunk = (chunk_t*)malloc(sizeof(chunk_t));
    if (chunk == NULL)
      EXIT_TRACE("Memory allocation failed.\n");
    r = mbuffer_create(&chunk->uncompressed_data, MAXBUF + bytes_left);
    if (r != 0) {
      EXIT_TRACE("Unable to initialize memory buffer.\n");
    }
    chunk->header.state = CHUNK_STATE_UNCOMPRESSED;
    if (bytes_left > 0) {
      // FIXME: Short-circuit this if no more data available

      //"Extension" of existing buffer, copy sequence number and left over data
      // to beginning of new buffer
      // NOTE: We cannot safely extend the current memory region because it has
      // already been given to another thread
      memcpy(chunk->uncompressed_data.ptr, temp->uncompressed_data.ptr,
             temp->uncompressed_data.n);
      mbuffer_free(&temp->uncompressed_data);
      free(temp);
      temp = NULL;
    }
    // Read data until buffer full
    size_t bytes_read = 0;
    if (conf->preloading) {
      size_t max_read =
          MIN(MAXBUF, args->input_file.size - preloading_buffer_seek);
      memcpy(chunk->uncompressed_data.ptr + bytes_left,
             args->input_file.buffer + preloading_buffer_seek, max_read);
      bytes_read = max_read;
      preloading_buffer_seek += max_read;
    } else {
      while (bytes_read < MAXBUF) {
        r = read(fd, chunk->uncompressed_data.ptr + bytes_left + bytes_read,
                 MAXBUF - bytes_read);
        if (r < 0)
          switch (errno) {
          case EAGAIN:
            EXIT_TRACE("I/O error: No data available\n");
            break;
          case EBADF:
            EXIT_TRACE("I/O error: Invalid file descriptor\n");
            break;
          case EFAULT:
            EXIT_TRACE("I/O error: Buffer out of range\n");
            break;
          case EINTR:
            EXIT_TRACE("I/O error: Interruption\n");
            break;
          case EINVAL:
            EXIT_TRACE("I/O error: Unable to read from file descriptor\n");
            break;
          case EIO:
            EXIT_TRACE("I/O error: Generic I/O error\n");
            break;
          case EISDIR:
            EXIT_TRACE("I/O error: Cannot read from a directory\n");
            break;
          default:
            EXIT_TRACE("I/O error: Unrecognized error\n");
            break;
          }
        if (r == 0)
          break;
        bytes_read += r;
      }
    }
    // No data left over from last iteration and also nothing new read in,
    // simply clean up and quit
    if (bytes_left + bytes_read == 0) {
      mbuffer_free(&chunk->uncompressed_data);
      free(chunk);
      chunk = NULL;
      break;
    }
    // Shrink buffer to actual size
    if (bytes_left + bytes_read < chunk->uncompressed_data.n) {
      r = mbuffer_realloc(&chunk->uncompressed_data, bytes_left + bytes_read);
      assert(r == 0);
    }

    // Check whether any new data was read in, process last chunk if not
    if (bytes_read == 0) {
      // Deduplicate
      int isDuplicate = sub_Deduplicate(chunk);

      // If chunk is unique compress & archive it.
      if (!isDuplicate) {
        sub_Compress(chunk);
      }

      write_chunk_to_file(fd_out, chunk);
      if (chunk->header.isDuplicate) {
        free(chunk);
        chunk = NULL;
      }

      // stop fetching from input buffer, terminate processing
      break;
    }

    // partition input block into fine-granular chunks
    int split;
    do {
      split = 0;
      // Try to split the buffer
      int offset =
          rabinseg(chunk->uncompressed_data.ptr, chunk->uncompressed_data.n,
                   rf_win_dataprocess, rabintab, rabinwintab);
      // Did we find a split location?
      if (offset == 0) {
        // Split found at the very beginning of the buffer (should never happen
        // due to technical limitations)
        assert(0);
        split = 0;
      } else if (offset < chunk->uncompressed_data.n) {
        // Split found somewhere in the middle of the buffer
        // Allocate a new chunk and create a new memory buffer
        temp = (chunk_t*)malloc(sizeof(chunk_t));
        if (temp == NULL)
          EXIT_TRACE("Memory allocation failed.\n");

        // split it into two pieces
        r = mbuffer_split(&chunk->uncompressed_data, &temp->uncompressed_data,
                          offset);
        if (r != 0)
          EXIT_TRACE("Unable to split memory buffer.\n");
        temp->header.state = CHUNK_STATE_UNCOMPRESSED;

        // Deduplicate
        int isDuplicate = sub_Deduplicate(chunk);

        // If chunk is unique compress & archive it.
        if (!isDuplicate) {
          sub_Compress(chunk);
        }

        write_chunk_to_file(fd_out, chunk);
        if (chunk->header.isDuplicate) {
          free(chunk);
          chunk = NULL;
        }

        // prepare for next iteration
        chunk = temp;
        temp  = NULL;
        split = 1;
      } else {
        // Due to technical limitations we can't distinguish the cases "no
        // split" and "split at end of buffer"
        // This will result in some unnecessary (and unlikely) work but yields
        // the correct result eventually.
        temp  = chunk;
        chunk = NULL;
        split = 0;
      }
    } while (split);
  }

  free(rabintab);
  free(rabinwintab);

  close(fd_out);

  return NULL;
}

/*
 * Pipeline stage function of fragmentation stage
 *
 * Actions performed:
 *  - Read data from file (or preloading buffer)
 *  - Perform coarse-grained chunking
 *  - Send coarse chunks to refinement stages for further processing
 *
 * Notes:
 * This pipeline stage is a bottleneck because it is inherently serial. We
 * therefore perform only coarse chunking and pass on the data block as fast
 * as possible so that there are no delays that might decrease scalability.
 * With very large numbers of threads this stage will not be able to keep up
 * which will eventually limit scalability. A solution to this is to increase
 * the size of coarse-grained chunks with a comparable increase in total
 * input size.
 */
void* Fragment(void* targs) {
  struct thread_args* args      = (struct thread_args*)targs;
  size_t preloading_buffer_seek = 0;
  int fd                        = args->fd;
  int i;

  ringbuffer_t send_buf;
  sequence_number_t anchorcount = 0;
  int r;

  chunk_t* temp       = NULL;
  chunk_t* chunk      = NULL;
  u32int* rabintab    = malloc(256 * sizeof rabintab[0]);
  u32int* rabinwintab = malloc(256 * sizeof rabintab[0]);
  if (rabintab == NULL || rabinwintab == NULL) {
    EXIT_TRACE("Memory allocation failed.\n");
  }

  r = ringbuffer_init(&send_buf, ANCHOR_DATA_PER_INSERT);
  assert(r == 0);

  rf_win_dataprocess = 0;
  rabininit(rf_win_dataprocess, rabintab, rabinwintab);

  // Sanity check
  if (MAXBUF < 8 * ANCHOR_JUMP) {
    printf("WARNING: I/O buffer size is very small. Performance degraded.\n");
    fflush(NULL);
  }

  // read from input file / buffer
  while (1) {
    size_t bytes_left; // amount of data left over in last_mbuffer from previous
                       // iteration

    // Check how much data left over from previous iteration resp. create an
    // initial chunk
    if (temp != NULL) {
      bytes_left = temp->uncompressed_data.n;
    } else {
      bytes_left = 0;
    }

    // Make sure that system supports new buffer size
    if (MAXBUF + bytes_left > SSIZE_MAX) {
      EXIT_TRACE("Input buffer size exceeds system maximum.\n");
    }
    // Allocate a new chunk and create a new memory buffer
    chunk = (chunk_t*)malloc(sizeof(chunk_t));
    if (chunk == NULL)
      EXIT_TRACE("Memory allocation failed.\n");
    r = mbuffer_create(&chunk->uncompressed_data, MAXBUF + bytes_left);
    if (r != 0) {
      EXIT_TRACE("Unable to initialize memory buffer.\n");
    }
    if (bytes_left > 0) {
      // FIXME: Short-circuit this if no more data available

      //"Extension" of existing buffer, copy sequence number and left over data
      // to beginning of new buffer
      chunk->header.state   = CHUNK_STATE_UNCOMPRESSED;
      chunk->sequence.l1num = temp->sequence.l1num;

      // NOTE: We cannot safely extend the current memory region because it has
      // already been given to another thread
      memcpy(chunk->uncompressed_data.ptr, temp->uncompressed_data.ptr,
             temp->uncompressed_data.n);
      mbuffer_free(&temp->uncompressed_data);
      free(temp);
      temp = NULL;
    } else {
      // brand new mbuffer, increment sequence number
      chunk->header.state   = CHUNK_STATE_UNCOMPRESSED;
      chunk->sequence.l1num = anchorcount;
      anchorcount++;
    }
    // Read data until buffer full
    size_t bytes_read = 0;
    if (conf->preloading) {
      size_t max_read =
          MIN(MAXBUF, args->input_file.size - preloading_buffer_seek);
      memcpy(chunk->uncompressed_data.ptr + bytes_left,
             args->input_file.buffer + preloading_buffer_seek, max_read);
      bytes_read = max_read;
      preloading_buffer_seek += max_read;
    } else {
      while (bytes_read < MAXBUF) {
        r = read(fd, chunk->uncompressed_data.ptr + bytes_left + bytes_read,
                 MAXBUF - bytes_read);
        if (r < 0)
          switch (errno) {
          case EAGAIN:
            EXIT_TRACE("I/O error: No data available\n");
            break;
          case EBADF:
            EXIT_TRACE("I/O error: Invalid file descriptor\n");
            break;
          case EFAULT:
            EXIT_TRACE("I/O error: Buffer out of range\n");
            break;
          case EINTR:
            EXIT_TRACE("I/O error: Interruption\n");
            break;
          case EINVAL:
            EXIT_TRACE("I/O error: Unable to read from file descriptor\n");
            break;
          case EIO:
            EXIT_TRACE("I/O error: Generic I/O error\n");
            break;
          case EISDIR:
            EXIT_TRACE("I/O error: Cannot read from a directory\n");
            break;
          default:
            EXIT_TRACE("I/O error: Unrecognized error\n");
            break;
          }
        if (r == 0)
          break;
        bytes_read += r;
      }
    }
    // No data left over from last iteration and also nothing new read in,
    // simply clean up and quit
    if (bytes_left + bytes_read == 0) {
      mbuffer_free(&chunk->uncompressed_data);
      free(chunk);
      chunk = NULL;
      break;
    }
    // Shrink buffer to actual size
    if (bytes_left + bytes_read < chunk->uncompressed_data.n) {
      r = mbuffer_realloc(&chunk->uncompressed_data, bytes_left + bytes_read);
      assert(r == 0);
    }
    // Check whether any new data was read in, enqueue last chunk if not
    if (bytes_read == 0) {
      // put it into send buffer
      r = ringbuffer_insert(&send_buf, chunk);
      assert(r == 0);
      // NOTE: No need to empty a full send_buf, we will break now and pass
      // everything on to the queue
      break;
    }
    // partition input block into large, coarse-granular chunks
    int split;
    do {
      split = 0;
      // Try to split the buffer at least ANCHOR_JUMP bytes away from its
      // beginning
      if (ANCHOR_JUMP < chunk->uncompressed_data.n) {
        int offset = rabinseg(chunk->uncompressed_data.ptr + ANCHOR_JUMP,
                              chunk->uncompressed_data.n - ANCHOR_JUMP,
                              rf_win_dataprocess, rabintab, rabinwintab);
        // Did we find a split location?
        if (offset == 0) {
          // Split found at the very beginning of the buffer (should never
          // happen due to technical limitations)
          assert(0);
          split = 0;
        } else if (offset + ANCHOR_JUMP < chunk->uncompressed_data.n) {
          // Split found somewhere in the middle of the buffer
          // Allocate a new chunk and create a new memory buffer
          temp = (chunk_t*)malloc(sizeof(chunk_t));
          if (temp == NULL)
            EXIT_TRACE("Memory allocation failed.\n");

          // split it into two pieces
          r = mbuffer_split(&chunk->uncompressed_data, &temp->uncompressed_data,
                            offset + ANCHOR_JUMP);
          if (r != 0)
            EXIT_TRACE("Unable to split memory buffer.\n");
          temp->header.state   = CHUNK_STATE_UNCOMPRESSED;
          temp->sequence.l1num = anchorcount;
          anchorcount++;

          // put it into send buffer
          r = ringbuffer_insert(&send_buf, chunk);
          assert(r == 0);

          // send a group of items into the next queue in round-robin fashion
          if (ringbuffer_isFull(&send_buf)) {
            // r = queue_enqueue(refine_que, &send_buf, ANCHOR_DATA_PER_INSERT);
            r = enqueue_from_ringbuffer(queue_refine, &send_buf,
                                        ANCHOR_DATA_PER_INSERT);
            assert(r >= 1);
          }
          // prepare for next iteration
          chunk = temp;
          temp  = NULL;
          split = 1;
        } else {
          // Due to technical limitations we can't distinguish the cases "no
          // split" and "split at end of buffer"
          // This will result in some unnecessary (and unlikely) work but yields
          // the correct result eventually.
          temp  = chunk;
          chunk = NULL;
          split = 0;
        }
      } else {
        // NOTE: We don't process the stub, instead we try to read in more data
        // so we might be able to find a proper split.
        //      Only once the end of the file is reached do we get a genuine
        //      stub which will be enqueued right after the read operation.
        temp  = chunk;
        chunk = NULL;
        split = 0;
      }
    } while (split);
  }

  // drain buffer
  while (!ringbuffer_isEmpty(&send_buf)) {
    // r = queue_enqueue(refine_que, &send_buf, ANCHOR_DATA_PER_INSERT);
    r = enqueue_from_ringbuffer(queue_refine, &send_buf,
                                ANCHOR_DATA_PER_INSERT);
    assert(r >= 1);
  }

  free(rabintab);
  free(rabinwintab);
  ringbuffer_destroy(&send_buf);

  // shutdown
  // queue_terminate(refine_que);
  terminate_queue(queue_refine);

  return NULL;
}

/*
 * Pipeline stage function of reorder stage
 *
 * Actions performed:
 *  - Receive chunks from compression and deduplication stage
 *  - Check sequence number of each chunk to determine correct order
 *  - Cache chunks that arrive out-of-order until predecessors are available
 *  - Write chunks in-order to file (or preloading buffer)
 *
 * Notes:
 *  - This function blocks if the compression stage has not finished supplying
 *    the compressed data for a duplicate chunk.
 */
void* Reorder(void* targs) {
  struct thread_args* args = (struct thread_args*)targs;
  int fd                   = 0;

  ringbuffer_t recv_buf;
  chunk_t* chunk;

  SearchTree T;
  T            = TreeMakeEmpty(NULL);
  Position pos = NULL;
  struct tree_element tele;

  sequence_t next;
  sequence_reset(&next);

  // We perform global anchoring in the first stage and refine the anchoring
  // in the second stage. This array keeps track of the number of chunks in
  // a coarse chunk.
  sequence_number_t* chunks_per_anchor;
  unsigned int chunks_per_anchor_max = 1024;
  chunks_per_anchor = malloc(chunks_per_anchor_max * sizeof(sequence_number_t));
  if (chunks_per_anchor == NULL)
    EXIT_TRACE("Error allocating memory\n");
  memset(chunks_per_anchor, 0,
         chunks_per_anchor_max * sizeof(sequence_number_t));
  int r;
  int i;

  r = ringbuffer_init(&recv_buf, ITEM_PER_FETCH);
  assert(r == 0);

  fd = create_output_file(conf->outfile);

  while (1) {
    // get a group of items
    if (ringbuffer_isEmpty(&recv_buf)) {
      // process queues in round-robin fashion
      // r = queue_dequeue(reorder_que, &recv_buf, ITEM_PER_FETCH);
      r = dequeue_to_ringbuffer(queue_reorder, &recv_buf, ITEM_PER_FETCH);
      if (r < 0)
        break;
    }
    chunk = (chunk_t*)ringbuffer_remove(&recv_buf);
    if (chunk == NULL)
      break;

    // Double size of sequence number array if necessary
    if (chunk->sequence.l1num >= chunks_per_anchor_max) {
      chunks_per_anchor =
          realloc(chunks_per_anchor,
                  2 * chunks_per_anchor_max * sizeof(sequence_number_t));
      if (chunks_per_anchor == NULL)
        EXIT_TRACE("Error allocating memory\n");
      memset(&chunks_per_anchor[chunks_per_anchor_max], 0,
             chunks_per_anchor_max * sizeof(sequence_number_t));
      chunks_per_anchor_max *= 2;
    }
    // Update expected L2 sequence number
    if (chunk->isLastL2Chunk) {
      assert(chunks_per_anchor[chunk->sequence.l1num] == 0);
      chunks_per_anchor[chunk->sequence.l1num] = chunk->sequence.l2num + 1;
    }

    // Put chunk into local cache if it's not next in the sequence
    if (!sequence_eq(chunk->sequence, next)) {
      pos = TreeFind(chunk->sequence.l1num, T);
      if (pos == NULL) {
        // FIXME: Can we remove at least one of the two mallocs in this
        // if-clause?
        // FIXME: Rename "INITIAL_SEARCH_TREE_SIZE" to something more accurate
        tele.l1num = chunk->sequence.l1num;
        tele.queue = Initialize(INITIAL_SEARCH_TREE_SIZE);
        Insert(chunk, tele.queue);
        T = TreeInsert(tele, T);
      } else {
        Insert(chunk, pos->Element.queue);
      }
      continue;
    }

    // write as many chunks as possible, current chunk is next in sequence
    pos = TreeFindMin(T);
    do {
      write_chunk_to_file(fd, chunk);
      if (chunk->header.isDuplicate) {
        free(chunk);
        chunk = NULL;
      }
      sequence_inc_l2(&next);
      if (chunks_per_anchor[next.l1num] != 0 &&
          next.l2num == chunks_per_anchor[next.l1num])
        sequence_inc_l1(&next);

      // Check whether we can write more chunks from cache
      if (pos != NULL && (pos->Element.l1num == next.l1num)) {
        chunk = FindMin(pos->Element.queue);
        if (sequence_eq(chunk->sequence, next)) {
          // Remove chunk from cache, update position for next iteration
          DeleteMin(pos->Element.queue);
          if (IsEmpty(pos->Element.queue)) {
            Destroy(pos->Element.queue);
            T   = TreeDelete(pos->Element, T);
            pos = TreeFindMin(T);
          }
        } else {
          // level 2 sequence number does not match
          chunk = NULL;
        }
      } else {
        // level 1 sequence number does not match or no chunks left in cache
        chunk = NULL;
      }
    } while (chunk != NULL);
  }

  // flush the blocks left in the cache to file
  pos = TreeFindMin(T);
  while (pos != NULL) {
    if (pos->Element.l1num == next.l1num) {
      chunk = FindMin(pos->Element.queue);
      if (sequence_eq(chunk->sequence, next)) {
        // Remove chunk from cache, update position for next iteration
        DeleteMin(pos->Element.queue);
        if (IsEmpty(pos->Element.queue)) {
          Destroy(pos->Element.queue);
          T   = TreeDelete(pos->Element, T);
          pos = TreeFindMin(T);
        }
      } else {
        // TODO: level 2 sequence number does not match
        // EXIT_TRACE("L2 sequence number mismatch.\n");
      }
    } else {
      // TODO: level 1 sequence number does not match
      // EXIT_TRACE("L1 sequence number mismatch.\n");
    }
    write_chunk_to_file(fd, chunk);
    if (chunk->header.isDuplicate) {
      free(chunk);
      chunk = NULL;
    }
    sequence_inc_l2(&next);
    if (chunks_per_anchor[next.l1num] != 0 &&
        next.l2num == chunks_per_anchor[next.l1num])
      sequence_inc_l1(&next);
  }

  close(fd);

  ringbuffer_destroy(&recv_buf);
  free(chunks_per_anchor);

  return NULL;
}

/*--------------------------------------------------------------------------*/
/* Encode
 * Compress an input stream
 *
 * Arguments:
 *   conf:    Configuration parameters
 *
 */
void Encode(config_t* _conf) {
  struct stat filestat;
  int32 fd;

  conf = _conf;

  // Create chunk cache
  cache = hashtable_create(65536, hash_from_key_fn, keys_equal_fn, FALSE);
  if (cache == NULL) {
    printf("ERROR: Out of memory\n");
    exit(1);
  }

  struct thread_args data_process_args;
  int i;

  assert(!mbuffer_system_init());

  /* src file stat */
  if (stat(conf->infile, &filestat) < 0)
    EXIT_TRACE("stat() %s failed: %s\n", conf->infile, strerror(errno));

  if (!S_ISREG(filestat.st_mode))
    EXIT_TRACE("not a normal file: %s\n", conf->infile);

  /* src file open */
  if ((fd = open(conf->infile, O_RDONLY | O_LARGEFILE)) < 0)
    EXIT_TRACE("%s file open error %s\n", conf->infile, strerror(errno));

  // Load entire file into memory if requested by user
  void* preloading_buffer = NULL;
  if (conf->preloading) {
    size_t bytes_read = 0;
    int r;

    preloading_buffer = malloc(filestat.st_size);
    if (preloading_buffer == NULL)
      EXIT_TRACE("Error allocating memory for input buffer.\n");

    // Read data until buffer full
    while (bytes_read < filestat.st_size) {
      r = read(fd, preloading_buffer + bytes_read,
               filestat.st_size - bytes_read);
      if (r < 0)
        switch (errno) {
        case EAGAIN:
          EXIT_TRACE("I/O error: No data available\n");
          break;
        case EBADF:
          EXIT_TRACE("I/O error: Invalid file descriptor\n");
          break;
        case EFAULT:
          EXIT_TRACE("I/O error: Buffer out of range\n");
          break;
        case EINTR:
          EXIT_TRACE("I/O error: Interruption\n");
          break;
        case EINVAL:
          EXIT_TRACE("I/O error: Unable to read from file descriptor\n");
          break;
        case EIO:
          EXIT_TRACE("I/O error: Generic I/O error\n");
          break;
        case EISDIR:
          EXIT_TRACE("I/O error: Cannot read from a directory\n");
          break;
        default:
          EXIT_TRACE("I/O error: Unrecognized error\n");
          break;
        }
      if (r == 0)
        break;
      bytes_read += r;
    }
    data_process_args.input_file.size   = filestat.st_size;
    data_process_args.input_file.buffer = preloading_buffer;
  }

  /* Variables for 3 thread pools and 2 pipeline stage threads.
   * The first and the last stage are serial (mostly I/O).
   */
  pthread_t threads_anchor[MAX_THREADS], threads_chunk[MAX_THREADS],
      threads_compress[MAX_THREADS], threads_send, threads_process;

  data_process_args.tid = 0;
  data_process_args.fd  = fd;

  // thread for first pipeline stage (input)
  pthread_create(&threads_process, NULL, Fragment, &data_process_args);

  // Create 3 thread pools for the intermediate pipeline stages
  struct thread_args anchor_thread_args[conf->nthreads];
  for (i = 0; i < conf->nthreads; i++) {
    anchor_thread_args[i].tid = i;
    pthread_create(&threads_anchor[i], NULL, FragmentRefine,
                   &anchor_thread_args[i]);
  }

  struct thread_args chunk_thread_args[conf->nthreads];
  for (i = 0; i < conf->nthreads; i++) {
    chunk_thread_args[i].tid = i;
    pthread_create(&threads_chunk[i], NULL, Deduplicate, &chunk_thread_args[i]);
  }

  struct thread_args compress_thread_args[conf->nthreads];
  for (i = 0; i < conf->nthreads; i++) {
    compress_thread_args[i].tid = i;
    pthread_create(&threads_compress[i], NULL, Compress,
                   &compress_thread_args[i]);
  }

  // thread for last pipeline stage (output)
  struct thread_args send_block_args;
  send_block_args.tid = 0;
  pthread_create(&threads_send, NULL, Reorder, &send_block_args);

  /*** parallel phase ***/

  // Return values of threads
  stats_t* threads_anchor_rv[conf->nthreads];
  stats_t* threads_chunk_rv[conf->nthreads];
  stats_t* threads_compress_rv[conf->nthreads];

  // join all threads
  pthread_join(threads_process, NULL);
  for (i = 0; i < conf->nthreads; i++)
    pthread_join(threads_anchor[i], (void**)&threads_anchor_rv[i]);
  for (i = 0; i < conf->nthreads; i++)
    pthread_join(threads_chunk[i], (void**)&threads_chunk_rv[i]);
  for (i = 0; i < conf->nthreads; i++)
    pthread_join(threads_compress[i], (void**)&threads_compress_rv[i]);
  pthread_join(threads_send, NULL);
}
