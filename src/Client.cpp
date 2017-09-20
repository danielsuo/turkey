#include "Client.h"
#include <glog/logging.h>
#include <iostream>
#include <sstream>

namespace Turkey {
Client::Client(const char* address,
               std::function<void(const Message*)> handler)
    : address_(address), rec_(0), context_(1), socket_(context_, ZMQ_DEALER), handler_(handler) {
  std::stringstream ss;
  ss << getpid();
  socket_.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());
}

Client::~Client() {
  LOG(INFO) << "Disconnecting from server (client " << getpid() << ")";
  sendMessage(MessageType_Stop, getpid());
  socket_.disconnect(address_);
}

void Client::start() {
  LOG(INFO) << "Connecting to server (client " << getpid() << ")";
  socket_.connect(address_);
  sendMessage(MessageType_Start, getpid());

  while(true) {
    recvAndProcessMessage();
  }
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

void Client::recvAndProcessMessage() {
  zmq::message_t header;
  zmq::message_t message;

  socket_.recv(&header, ZMQ_RCVMORE);
  socket_.recv(&message);

  auto msg = GetMessage(message.data());

  handler_(msg);
}

void Client::setHandler(std::function<void(const Message*)> handler) {
  handler_ = handler;
}
}
