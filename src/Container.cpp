#include "Container.h"

namespace docker {

Container::Container(std::string args_path) {
  std::ifstream i(args_path);
  i >> _args;

  std::cout << _args.dump() << std::endl;

  Response res = Docker::GetInstance().POST(getURL("create", false),
                                            _args.dump());

  json data = json::parse(res.data);

  if (res.code != 201) {
    fprintf(stderr, "ERROR: unable to create container");
    std::cerr << data["message"] << std::endl;
    exit(1);
  }

  std::cout << "Container successfully created!" << std::endl;

  _id = data["Id"];
  _which = PRIO_PROCESS;
}

std::string Container::getURL(std::string endpoint,
                              bool use_id,
                              std::string params) {
  std::stringstream ss;

  ss << "containers";

  if (use_id) {
    ss << "/" << _id;
  }

  if (endpoint != "") {
    ss << "/" << endpoint;
  }

  if (params != "") {
    ss << "?" << params;
  }

  return ss.str();
}

Response Container::attach() {
  std::cout << "Attaching to: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("attach", true));
  std::cout << res.code << std::endl;

  return res;
}

Response Container::start() {
  std::cout << "Starting: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("start", true));
  std::cout << res.code << std::endl;

  // If the container is started or was already running, grab _pid
  if (res.code == 204 || res.code == 304) {
    Response insp = inspect();

    if (insp.code == 200) {
      json data = json::parse(insp.data);
      _pid = data["State"]["Pid"];

      std::cout << "_pid: " << _pid << std::endl;
    }
  }

  return res;
}

Response Container::stop() {
  std::cout << "Starting: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("stop", true));
  std::cout << res.code << std::endl;

  return res;
}

Response Container::remove() {
  std::cout << "Removing: " << _id << std::endl;
  Response res = Docker::GetInstance().DELETE(getURL("", true));
  std::cout << res.code << std::endl;

  return res;
}

Response Container::inspect() {
  std::cout << "Inspecting: " << _id << std::endl;
  Response res = Docker::GetInstance().GET(getURL("json", true));
  std::cout << res.code << std::endl;

  return res;
}

// TODO: actually implement query parameters
Response Container::logs(bool follow, bool stdout, bool stderr, int since, bool timestamps, int tail) {
  std::cout << "Fetching logs: " << _id << std::endl;
  // Response res = POST("logs");
  Response res = Docker::GetInstance().GET(getURL("logs", true, "stdout=true&stderr=true"));
  std::cout << res.code << std::endl;

  return res;
}

void Container::signal(int signum) {
  if (kill(_pid, signum) < 0) {
    perror("ERROR: failed to signal");
  }
}

// TODO: this should really set _mask
void Container::setaffinity(cpu_set_t *mask) {
  if (sched_setaffinity(_pid, sizeof(cpu_set_t), mask) < 0) {
    perror("ERROR: failed to set affinity");
  }
}


// TODO: this should really just return _mask
void Container::getaffinity(cpu_set_t *mask) {
  if (sched_getaffinity(_pid, sizeof(cpu_set_t), mask) < 0) {
    perror("ERROR: failed to get affinity");
  }
}

void Container::setpriority(int priority_offset) {
  if (::setpriority(_which, _pid, priority_offset) < 0) {
    perror("ERROR: failed to set priority");
  }

  _priority += priority_offset;
}

// TODO: this should really just return _priority
int Container::getpriority() {
  int priority = 0;
  
  if ((priority = ::getpriority(_which, _pid)) == -1) {
    perror("ERROR: failed to get priority");
  }

  return priority;
}

}
