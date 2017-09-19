#pragma once
#include "Common.h"
#include <folly/Optional.h>

#include "fbs/fbs.h"
#include <zmq.hpp>

namespace Turkey {
class Client {
public:
  explicit Client(const char* const address);
  ~Client();

  // Delete copy ctor and copy-assignment ctor
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  // size_t pollServer();

  void sendMessage(MessageType type, int data);
  const Message* recvAndProcessMessage();

private:
  size_t rec_;
  zmq::context_t context_;
  zmq::socket_t socket_;

  // void registerWithServer();
  // boost::uuids::uuid id_;
  // RecInfo rec_;
  // bool registered_ = false;
};
}
