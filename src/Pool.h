#pragma once
#include "Client.h"
#include <folly/experimental/FunctionScheduler.h>
#include <iostream>
#include <wangle/concurrent/ThreadPoolExecutor.h>
using namespace wangle;

namespace Turkey {
// class DynamicThreadPool : public wangle::DynamicThreadPoolExecutor {
// public:
//   explicit DynamicThreadPool(size_t defaultNumThreads);
//   ~DynamicThreadPool();
//
//   // wangle::DynamicThreadPoolExecutor& getPool() { return pool_; }
//
// private:
//   void updatePoolSize();
//   size_t currentNumThreads_;
//   Client client_;
//   // wangle::DynamicThreadPoolExecutor pool_;
//   folly::FunctionScheduler fs_;
// };

/**
 * A Thread pool for CPU bound tasks.
 *
 * @note A single queue backed by folly/LifoSem and folly/MPMC queue.
 * Because of this contention can be quite high,
 * since all the worker threads and all the producer threads hit
 * the same queue. MPMC queue excels in this situation but dictates a max queue
 * size
 *
 * @note LifoSem wakes up threads in Lifo order - i.e. there are only few
 * threads as necessary running, and we always try to reuse the same few threads
 * for better cache locality.
 * Inactive threads have their stack madvised away. This works quite well in
 * combination with Lifosem - it almost doesn't matter if more threads than are
 * necessary are specified at startup.
 *
 * @note stop() will finish all outstanding tasks at exit
 *
 * @note Supports priorities - priorities are implemented as multiple queues -
 * each worker thread checks the highest priority queue first. Threads
 * themselves don't have priorities set, so a series of long running low
 * priority tasks could still hog all the threads. (at last check pthreads
 * thread priorities didn't work very well)
 */
class DynamicThreadPoolExecutor : public ThreadPoolExecutor {
public:
  struct CPUTask;

  DynamicThreadPoolExecutor(
      size_t numThreads, std::unique_ptr<BlockingQueue<CPUTask>> taskQueue,
      std::shared_ptr<ThreadFactory> threadFactory =
          std::make_shared<NamedThreadFactory>("DynamicThreadPool"));

  explicit DynamicThreadPoolExecutor(size_t numThreads);

  DynamicThreadPoolExecutor(size_t numThreads,
                            std::shared_ptr<ThreadFactory> threadFactory);

  DynamicThreadPoolExecutor(
      size_t numThreads, int8_t numPriorities,
      std::shared_ptr<ThreadFactory> threadFactory =
          std::make_shared<NamedThreadFactory>("DynamicThreadPool"));

  DynamicThreadPoolExecutor(
      size_t numThreads, int8_t numPriorities, size_t maxQueueSize,
      std::shared_ptr<ThreadFactory> threadFactory =
          std::make_shared<NamedThreadFactory>("DynamicThreadPool"));

  ~DynamicThreadPoolExecutor();

  void add(folly::Func func) override;
  void add(folly::Func func, std::chrono::milliseconds expiration,
           folly::Func expireCallback = nullptr) override;

  void addWithPriority(folly::Func func, int8_t priority) override;
  void add(folly::Func func, int8_t priority,
           std::chrono::milliseconds expiration,
           folly::Func expireCallback = nullptr);

  uint8_t getNumPriorities() const override;

  struct CPUTask : public ThreadPoolExecutor::Task {
    // Must be noexcept move constructible so it can be used in MPMCQueue

    explicit CPUTask(folly::Func&& f, std::chrono::milliseconds expiration,
                     folly::Func&& expireCallback)
        : Task(std::move(f), expiration, std::move(expireCallback)),
          poison(false) {}
    CPUTask()
        : Task(nullptr, std::chrono::milliseconds(0), nullptr), poison(true) {}

    bool poison;
  };

  static const size_t kDefaultMaxQueueSize;

protected:
  BlockingQueue<CPUTask>* getTaskQueue();

private:
  void threadRun(ThreadPtr thread) override;
  void stopThreads(size_t n) override;
  uint64_t getPendingTaskCount() override;
  void updatePoolSize();

  std::unique_ptr<BlockingQueue<CPUTask>> taskQueue_;
  std::atomic<ssize_t> threadsToStop_{0};

  size_t currentNumThreads_;
  // Client client_;
  // wangle::DynamicThreadPoolExecutor pool_;
  folly::FunctionScheduler fs_;
};
}
