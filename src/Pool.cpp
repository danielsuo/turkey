#include "Pool.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <glog/logging.h>
#include <iostream>

#include <wangle/concurrent/PriorityLifoSemMPMCQueue.h>

using folly::Func;

std::atomic<bool> turkey_client_quit(false);
static void turkey_client_got_signal(int) {
  LOG(ERROR) << "Got signal...";
  turkey_client_quit.store(true);
}

static void turkey_client_catch_signals() {
  LOG(INFO) << "Setting up signal catcher...";
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = turkey_client_got_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
}

namespace Turkey {
//
// DynamicThreadPool::DynamicThreadPool(size_t defaultNumThreads)
//     : wangle::DynamicThreadPoolExecutor(defaultNumThreads),
//       currentNumThreads_(defaultNumThreads), client_(defaultNumThreads) {
//   // Set up signal handler.
//   // turkey_client_catch_signals();
//
//   using namespace std::chrono_literals;
//   fs_.addFunction(std::bind(&DynamicThreadPool::updatePoolSize, this), 500ms,
//                   "updatePoolSize");
//   fs_.start();
// }
//
// void DynamicThreadPool::updatePoolSize() {
//
//   if (turkey_client_quit.load()) {
//     LOG(INFO) << "Wrapping up and dying";
//
//     fs_.cancelFunction("updatePoolSize");
//     // pool_.stop();
//     // delete this;
//   }
//
//   LOG(INFO) << "Updating pool size...";
//   size_t numThreads;
//   try {
//     numThreads = client_.pollServer();
//   } catch (const std::exception& ex) {
//     LOG(ERROR) << "Error in Turkey Client: " << ex.what();
//     return;
//   }
//
//   if (numThreads == currentNumThreads_) {
//     // Shortcircuit to avoid locking the thread list
//     return;
//   }
//
//   // pool_.setNumThreads(numThreads);
// }
//
// DynamicThreadPool::~DynamicThreadPool() {
//   LOG(INFO) << "Destroying DynamicThreadPool";
// }

const size_t DynamicThreadPoolExecutor::kDefaultMaxQueueSize = 1 << 14;

DynamicThreadPoolExecutor::DynamicThreadPoolExecutor(
    size_t numThreads, std::unique_ptr<BlockingQueue<CPUTask>> taskQueue,
    std::shared_ptr<ThreadFactory> threadFactory)
    : ThreadPoolExecutor(numThreads, std::move(threadFactory)),
      taskQueue_(std::move(taskQueue)) {

  turkey_client_catch_signals();
  using namespace std::chrono_literals;
  fs_.addFunction(std::bind(&DynamicThreadPool::updatePoolSize, this), 500ms,
                  "updatePoolSize");
  fs_.start();

  addThreads(numThreads);
  CHECK(threadList_.get().size() == numThreads);
}

DynamicThreadPoolExecutor::DynamicThreadPoolExecutor(
    size_t numThreads, std::shared_ptr<ThreadFactory> threadFactory)
    : DynamicThreadPoolExecutor(
          numThreads, std::make_unique<LifoSemMPMCQueue<CPUTask>>(
                          DynamicThreadPoolExecutor::kDefaultMaxQueueSize),
          std::move(threadFactory)) {}

DynamicThreadPoolExecutor::DynamicThreadPoolExecutor(size_t numThreads)
    : DynamicThreadPoolExecutor(
          numThreads, std::make_shared<NamedThreadFactory>("CPUThreadPool")) {}

DynamicThreadPoolExecutor::DynamicThreadPoolExecutor(
    size_t numThreads, int8_t numPriorities,
    std::shared_ptr<ThreadFactory> threadFactory)
    : DynamicThreadPoolExecutor(
          numThreads,
          std::make_unique<PriorityLifoSemMPMCQueue<CPUTask>>(
              numPriorities, DynamicThreadPoolExecutor::kDefaultMaxQueueSize),
          std::move(threadFactory)) {}

DynamicThreadPoolExecutor::DynamicThreadPoolExecutor(
    size_t numThreads, int8_t numPriorities, size_t maxQueueSize,
    std::shared_ptr<ThreadFactory> threadFactory)
    : DynamicThreadPoolExecutor(
          numThreads, std::make_unique<PriorityLifoSemMPMCQueue<CPUTask>>(
                          numPriorities, maxQueueSize),
          std::move(threadFactory)) {}

DynamicThreadPoolExecutor::~DynamicThreadPoolExecutor() {
  stop();
  CHECK(threadsToStop_ == 0);
}

void DynamicThreadPoolExecutor::add(Func func) {
  add(std::move(func), std::chrono::milliseconds(0));
}

void DynamicThreadPoolExecutor::add(Func func,
                                    std::chrono::milliseconds expiration,
                                    Func expireCallback) {
  // TODO handle enqueue failure, here and in other add() callsites
  taskQueue_->add(
      CPUTask(std::move(func), expiration, std::move(expireCallback)));
}

void DynamicThreadPoolExecutor::addWithPriority(Func func, int8_t priority) {
  add(std::move(func), priority, std::chrono::milliseconds(0));
}

void DynamicThreadPoolExecutor::add(Func func, int8_t priority,
                                    std::chrono::milliseconds expiration,
                                    Func expireCallback) {
  CHECK(getNumPriorities() > 0);
  taskQueue_->addWithPriority(
      CPUTask(std::move(func), expiration, std::move(expireCallback)),
      priority);
}

uint8_t DynamicThreadPoolExecutor::getNumPriorities() const {
  return taskQueue_->getNumPriorities();
}

BlockingQueue<DynamicThreadPoolExecutor::CPUTask>*
DynamicThreadPoolExecutor::getTaskQueue() {
  return taskQueue_.get();
}

void DynamicThreadPoolExecutor::threadRun(std::shared_ptr<Thread> thread) {
  thread->startupBaton.post();
  while (1) {
    auto task = taskQueue_->take();
    if (UNLIKELY(task.poison)) {
      CHECK(threadsToStop_-- > 0);
      for (auto& o : observers_) {
        o->threadStopped(thread.get());
      }
      folly::RWSpinLock::WriteHolder w{&threadListLock_};
      threadList_.remove(thread);
      stoppedThreads_.add(thread);
      return;
    } else {
      runTask(thread, std::move(task));
    }

    if (UNLIKELY(threadsToStop_ > 0 && !isJoin_)) {
      if (--threadsToStop_ >= 0) {
        folly::RWSpinLock::WriteHolder w{&threadListLock_};
        threadList_.remove(thread);
        stoppedThreads_.add(thread);
        return;
      } else {
        threadsToStop_++;
      }
    }
  }
}

void DynamicThreadPoolExecutor::stopThreads(size_t n) {
  threadsToStop_ += n;
  for (size_t i = 0; i < n; i++) {
    taskQueue_->addWithPriority(CPUTask(), Executor::HI_PRI);
  }
}

uint64_t DynamicThreadPoolExecutor::getPendingTaskCount() {
  return taskQueue_->size();
}
}
