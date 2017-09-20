#pragma once
#include "Common.h"
#include <folly/Optional.h>

#include "fbs/fbs.h"
#include <zmq.hpp>

namespace Turkey {
class Client {
public:
  explicit Client(const char* address,
                  std::function<void(const Message*)> handler =
                      [](const Message*) {});
  ~Client();

  // Delete copy ctor and copy-assignment ctor
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  void start();
  void stop();
  void sendMessage(MessageType type, int data);
  void recvAndProcessMessage();
  void setHandler(std::function<void(const Message*)> handler);

private:
  const char * address_;
  size_t rec_;
  zmq::context_t context_;
  zmq::socket_t socket_;
  std::function<void(const Message*)> handler_;
};
}
