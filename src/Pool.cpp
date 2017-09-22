#include "Pool.h"
#include <chrono>
#include <glog/logging.h>
#include <iostream>
namespace Turkey {

DynamicThreadPool::DynamicThreadPool(size_t defaultNumThreads)
    : currentNumThreads_(defaultNumThreads), client_("tcp://localhost:5555"),
      pool_(defaultNumThreads, 3) { // Make pool with three priorities
  client_.setHandler([this](const Message* msg) {
    switch (msg->type()) {
    case MessageType_Stop:
      LOG(INFO) << "Received Stop message from server";
      exit(0);
      break;
    case MessageType_Update:
      LOG(INFO) << "Received Update message from server with data " << msg->data();
      this->currentNumThreads_ = msg->data();
      this->pool_.setNumThreads(this->currentNumThreads_);
      break;
    }
  });
}

void DynamicThreadPool::start() {
  clientThread_ =
      std::unique_ptr<std::thread>(new std::thread(&Client::start, &client_));
}

// TODO: this is pretty drastic. Means stop() should be the last thing you call
// in your program. Such ugly. Much wow.
void DynamicThreadPool::stop() {
  client_.stop();
  clientThread_->join();
}
}
