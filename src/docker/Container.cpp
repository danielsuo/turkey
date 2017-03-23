#include "Container.h"

namespace docker {

Container::Container(std::string args_path) {
  std::ifstream i(args_path);
  i >> _args;

  // std::stringstream ss;
  // ss << "[\"" << TURKEY_SERVER_PID_KEY << "=" << getpid() << "\"]";
  // std::cerr << ss.str() << std::endl;
  // _args["Env"] = json::parse(ss.str());

  std::cerr << _args.dump() << std::endl;
}

// NOTE: We cannot destroy containers in time if the main process exits, so
// there needs to be an auxillary mechanism to clean up
Container::~Container() {
  // stop();
  // remove();

  fprintf(stderr, "Destructing Container %d.\n", _pid);
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

void Container::addArgs(std::string key, std::string value) {
  _args[key] = json::parse(value);
}

std::string Container::getIP() {
  return _ip;
}

Response Container::create() {
  Response res = Docker::GetInstance().POST(getURL("create", false),
                                            _args.dump());

  json data = json::parse(res.data);

  if (res.code != 201) {
    fprintf(stderr, "ERROR: unable to create container");
    std::cerr << data["message"] << std::endl;
    exit(1);
  }

  std::cerr << "Container successfully created!" << std::endl;

  _id = data["Id"];
  _which = PRIO_PROCESS;

  return res;
}

Response Container::attach() {
  std::cerr << "Attaching to: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("attach", true));
  std::cerr << res.code << std::endl;

  return res;
}

Response Container::start() {
  std::cerr << "Starting: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("start", true));
  std::cerr << res.code << std::endl;

  // If the container is started or was already running, grab _pid
  if (res.code == 204 || res.code == 304) {
    Response insp = inspect();

    if (insp.code == 200) {
      json data = json::parse(insp.data);
      _pid = data["State"]["Pid"];
      _ip = data["NetworkSettings"]["IPAddress"];
      inet_aton(_ip.c_str(), &_in_addr);

      std::cerr << "_pid: " << _pid << std::endl;
      std::cerr << "_ip: " << _ip << std::endl;
      std::cerr << "_s_addr: " << _in_addr.s_addr << std::endl;
    }
  }

  return res;
}

Response Container::stop() {
  std::cerr << "Stopping: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("stop", true));
  std::cerr << res.code << std::endl;

  return res;
}

Response Container::remove() {
  std::cerr << "Removing: " << _id << std::endl;
  Response res = Docker::GetInstance().DELETE(getURL("", true, "v=true&force=true"));
  std::cerr << res.code << std::endl;

  return res;
}

Response Container::inspect() {
  std::cerr << "Inspecting: " << _id << std::endl;
  Response res = Docker::GetInstance().GET(getURL("json", true));
  std::cerr << res.code << std::endl;

  return res;
}

// {
//   "BlkioWeight": 300,
//   "CpuShares": 512,
//   "CpuPeriod": 100000,
//   "CpuQuota": 50000,
//   "CpuRealtimePeriod": 1000000,
//   "CpuRealtimeRuntime": 10000,
//   "CpusetCpus": "0,1",
//   "CpusetMems": "0",
//   "Memory": 314572800,
//   "MemorySwap": 514288000,
//   "MemoryReservation": 209715200,
//   "KernelMemory": 52428800,
//   "RestartPolicy": {
//     "MaximumRetryCount": 4,
//     "Name": "on-failure"
//   }
// }
Response Container::update(json data) {
  std::cerr << "Updating: " << _id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("update", true), data.dump());
  std::cerr << res.code << std::endl;

  return res;
}

// TODO: actually implement query parameters
Response Container::logs(bool follow, bool stdout, bool stderr, int since, bool timestamps, int tail) {
  std::cerr << "Fetching logs: " << _id << std::endl;
  // Response res = POST("logs");
  std::stringstream ss;

  ss << "stdout=true&stderr=true";

  if (follow) {
    ss << "&follow=true";
  }

  Response res = Docker::GetInstance().GET(getURL("logs", true, ss.str()));
  std::cerr << res.code << std::endl;

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
