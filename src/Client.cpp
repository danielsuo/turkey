#include "Client.h"
#include <glog/logging.h>
#include <iostream>
#include <sstream>

namespace Turkey {
Client::Client(const char* const address)
    : rec_(0), context_(1), socket_(context_, ZMQ_DEALER) {
  std::stringstream ss;
  ss << getpid();
  this->socket_.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());

  LOG(INFO) << "Connecting to server from client " << getpid();
  this->socket_.connect(address);
}

Client::~Client() {}

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

  this->socket_.send(header, ZMQ_SNDMORE);
  this->socket_.send(encoded);
}

const Message* Client::recvAndProcessMessage() {
  zmq::message_t header;
  zmq::message_t message;

  this->socket_.recv(&header, ZMQ_RCVMORE);
  this->socket_.recv(&message);

  auto msg = GetMessage(message.data());
  LOG(INFO) << "Parsed message";
  LOG(INFO) << msg->data();

  return msg;
}

// Client::Client(size_t defaultRec)
// :id_(boost::uuids::random_generator()()) {
// RecInfo rec;
// rec.rec = defaultRec;
// rec_ = rec;

// registerWithServer();
// pollServer();
// }

// void Client::registerWithServer() {
// try {
// managed_shared_memory segment(open_only, "TurkeySharedMemory");
// named_mutex mutex(open_only, "TurkeyMutex");
// scoped_lock<named_mutex> lock(mutex);

// Get default recommendation to use as starting value
// const auto defaultRec = segment.find<size_t>("DefaultRec").first;
// RecInfo rec;
// rec.rec = *defaultRec;
// rec_ = rec;

// Register client in the vector
// auto recMap = segment.find<RecMap>("RecMap").first;
// recMap->insert(std::pair<const uuid, RecInfo>(id_, rec_));

// registered_ = true;
// LOG(INFO) << "Client registered. UUID: " << id_ << ". Rec: " << rec_.rec;
// } catch (const std::exception& ex) {
// LOG(INFO) << "Interprocess exception: " << ex.what();
// TODO any remediation?
// }
// }

// size_t Client::pollServer() {
// if (!registered_) {
// registerWithServer();
// }
// try {
// managed_shared_memory segment(open_only, "TurkeySharedMemory");
// named_mutex mutex(open_only, "TurkeyMutex");
// scoped_lock<named_mutex> lock(mutex);

// auto recMap = segment.find<RecMap>("RecMap").first;

// if (registered_) {
// TODO take the default
// const auto defaultRec = segment.find<size_t>("DefaultRec").first;
// RecInfo rec;
// rec.rec = *defaultRec;
// rec_ = rec;
// }
// } catch (const std::exception& ex) {
// LOG(INFO) << "Interprocess exception: " << ex.what();
// TODO any remediation?
// }
// return rec_.rec;
// }
}
