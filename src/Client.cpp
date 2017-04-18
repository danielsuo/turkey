#include "Client.h"
#include <glog/logging.h>
#include <iostream>

using namespace boost::interprocess;
using namespace boost::uuids;

namespace Turkey {
Client::Client(size_t defaultRec)
    : rec_(defaultRec), id_(boost::uuids::random_generator()()) {
  registerWithServer();
  pollServer();
}

void Client::registerWithServer() {
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    // Get default recommendation to use as starting value
    const auto rec = segment.find<size_t>("DefaultRec").first;
    rec_ = *rec;

    // Register client in the vector
    auto recMap = segment.find<RecMap>("RecMap").first;
    recMap->insert(std::pair<const uuid, size_t>(id_, rec_));

    registered_ = true;
    LOG(INFO) << "Client registered. UUID: " << id_ << ". Rec: " << rec_;
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
}

size_t Client::pollServer() {
  if (!registered_) {
    registerWithServer();
  }
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    auto recMap = segment.find<RecMap>("RecMap").first;

    if (registered_) {
      // TODO what happens when server crashes and restarts? need some
      // invalidation
      // rec_ = recMap->at(id_);
    }
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
  return rec_;
}
}
