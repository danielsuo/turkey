#include <folly/MPMCQueue.h>

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
