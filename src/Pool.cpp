#include "Pool.h"
#include <chrono>
#include <glog/logging.h>
#include <iostream>
namespace Turkey {

DynamicThreadPool::DynamicThreadPool(size_t defaultNumThreads)
    : currentNumThreads_(defaultNumThreads), client_("tcp://localhost:5555"),
      pool_(defaultNumThreads) {
  client_.setHandler([this](const Message* msg) {
    LOG(INFO) << "Got a message!";
    if (msg->type() == MessageType_Update) {
      LOG(INFO) << "Got update msg!";
      this->currentNumThreads_ = msg->data();
      this->pool_.setNumThreads(this->currentNumThreads_);
    }
  });
}

void DynamicThreadPool::start() {
  clientThread_ =
      std::unique_ptr<std::thread>(new std::thread(&Client::start, &client_));
}

// TODO: this is pretty drastic. Means stop() should be the last thing you call
// in your program. Such ugly. Much wow.
// TODO: Do this in a nice way
void DynamicThreadPool::stop() {
  LOG(INFO) << "NOTE: Do not be alarmed by the termination error below. This "
               "is terrible, yes, but we were in a hurry.";
  client_.~Client();
  std::terminate();
}
}
