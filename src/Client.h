#pragma once
#include <list>
#include <thread>

#include "fbs/fbs.h"
#include <folly/Optional.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <zmq.hpp>

namespace Turkey {
class Client {
public:
  explicit Client(int defaultNumThreads = 1, int numPools = 1,
                  const char* address = "tcp://localhost:5555",
                  std::function<void(const Message*)> allocator = nullptr);

  ~Client();

  // Delete copy ctor and copy-assignment ctor
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  void start();
  void stop();
  void sendMessage(MessageType type, int data);
  void processMessage();
  void setAllocator(std::function<void(const Message*)> allocator);
  void createPool(int defaultNumThreads = 1, int numPriorities = 3);
  wangle::CPUThreadPoolExecutor& getPool(int index = 0);

  // TODO: yeah, yeah, this should be private, but we don't have time for
  // accessors!
  std::list<wangle::CPUThreadPoolExecutor> pools;

private:
  const char* address_;
  size_t rec_;
  bool registered_;
  zmq::context_t context_;
  zmq::socket_t socket_;
  std::function<void(const Message*)> allocator_;
  std::unique_ptr<std::thread> messageProcessingThread_;
};
}
