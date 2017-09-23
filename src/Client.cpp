#include "Client.h"
#include <glog/logging.h>
#include <iostream>
#include <sstream>

namespace Turkey {

Client::Client(int defaultNumThreads, const char* address, int numPools,
               std::function<void(const Message*)> allocator)
    : address_(address), rec_(0), context_(1), socket_(context_, ZMQ_DEALER) {
  std::stringstream ss;
  ss << getpid();
  socket_.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());

  for (int i = 0; i < numPools; i++) {
    createPool(defaultNumThreads);
  }

  if (allocator) {
    setAllocator(allocator);
  } else {
    allocator_ = [this](const Message* msg) {
      for (auto& pool : this->pools) {
        pool.setNumThreads(msg->data());
      }
    };
  }
}

Client::~Client() {
  LOG(INFO) << "Disconnecting from server (client " << getpid() << ")";
  // TODO: Should do this for cleanliness
  // socket_.disconnect(address_);
}

void Client::start() {

  messageProcessingThread_ = std::unique_ptr<std::thread>(
      new std::thread(&Client::processMessage, this));
}

void Client::stop() {
  sendMessage(MessageType_Stop, getpid());
  messageProcessingThread_->join();
}

void Client::sendMessage(MessageType type, int data) {
  flatbuffers::FlatBufferBuilder fbb;
  MessageBuilder builder(fbb);

  builder.add_type(type);
  builder.add_data(data);
  auto message = builder.Finish();
  fbb.Finish(message);

  zmq::message_t header(0);
  zmq::message_t encoded(fbb.GetSize());
  memcpy(encoded.data(), fbb.GetBufferPointer(), fbb.GetSize());

  socket_.send(header, ZMQ_SNDMORE);
  socket_.send(encoded);
}

void Client::processMessage() {
  LOG(INFO) << "Connecting to server (client " << getpid() << ")";
  socket_.connect(address_);

  sendMessage(MessageType_Start, getpid());

  while (true) {
    zmq::message_t header;
    zmq::message_t message;

    socket_.recv(&header, ZMQ_RCVMORE);
    socket_.recv(&message);

    auto msg = GetMessage(message.data());

    switch (msg->type()) {
    case MessageType_Start:
      LOG(INFO) << "Received start message from server";
      break;
    case MessageType_Stop:
      LOG(INFO) << "Received stop message from server";
      exit(0);
      break;
    case MessageType_Update:
      LOG(INFO) << "Received Update message from server with data "
                << msg->data();
      allocator_(msg);
    }
  }
}

void Client::createPool(int defaultNumThreads, int numPriorities) {
  pools.emplace_back(defaultNumThreads, numPriorities);
}

void Client::setAllocator(std::function<void(const Message*)> allocator) {
  allocator_ = allocator;
}
}
