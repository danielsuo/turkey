#include "Client.h"
#include <glog/logging.h>
#include <iostream>

using namespace boost::interprocess;

namespace Turkey {

Client::Client(size_t defaultRec) : rec_(defaultRec) {
  registerWithServer();
  pollServer();
}

void Client::registerWithServer() {
  try {
    managed_shared_memory segment(open_only, TURKEY_SHM_KEY);
    named_mutex mutex(open_only, "TurkeyMutex");
    {
      scoped_lock<named_mutex> lock(mutex);

      // Get default recommendation to use as starting value
      const auto rec = segment.find<size_t>("DefaultRec").first;
      rec_ = *rec;

      // Register client in the vector
      auto recVec = segment.find<RecVec>("RecVec").first;
      // id_ = recVec->size();
      recVec->push_back(rec_);
    }

    id_ = incrementAndGetLastIDSeen();
    shmKey_ = getClientShmKey(*id_);
    mutexKey_ = getClientMutexKey(*id_);

    // TODO: this is symptomatic of bad design. Server should really handle this
    // so Client can misbehave, but client can die before the two can sync
    named_mutex shmMutex(open_only, mutexKey_.c_str());
    {
      scoped_lock<named_mutex> lock(shmMutex);
      auto shm = segment.construct<ClientShmStruct>(shmKey_.c_str())();
      shm->id = *id_;
      shm->alive = true;
    }

  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
}

size_t Client::pollServer() {
  if (!id_) {
    registerWithServer();
  }
  try {
    managed_shared_memory segment(open_only, TURKEY_SHM_KEY);
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    auto recVec = segment.find<RecVec>("RecVec").first;

    if (id_) {
      // TODO what happens when server crashes and restarts? need some
      // invalidation
      rec_ = recVec->at(*id_);
    }
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
  return rec_;
}

Client::~Client() {
  LOG(INFO) << "Quitting client";
  // TODO: having try in destructor? Gross.
  try {
    managed_shared_memory segment(open_only, TURKEY_SHM_KEY);
    if (!shared_memory_object::remove(shmKey_.c_str())) {
      std::cerr << "Failed to remove shared memory for client " << *id_
                << std::endl;
    }
    named_mutex::remove(mutexKey_.c_str());
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
}
}
