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

  id = data["Id"];
}

std::string Container::getURL(std::string endpoint,
                              bool use_id,
                              std::string params) {
  std::stringstream ss;

  ss << "containers";

  if (use_id) {
    ss << "/" << id;
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
  std::cout << "Attaching to: " << id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("attach", true));
  std::cout << res.code << std::endl;

  return res;
}

Response Container::start() {
  std::cout << "Starting: " << id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("start", true));
  std::cout << res.code << std::endl;

  // If the container is started or was already running, grab pid
  if (res.code == 204 || res.code == 304) {
    Response insp = inspect();

    if (insp.code == 200) {
      json data = json::parse(insp.data);
      pid = data["State"]["Pid"];

      std::cout << "pid: " << pid << std::endl;
    }
  }

  return res;
}

Response Container::stop() {
  std::cout << "Starting: " << id << std::endl;
  Response res = Docker::GetInstance().POST(getURL("stop", true));
  std::cout << res.code << std::endl;

  return res;
}

Response Container::remove() {
  std::cout << "Removing: " << id << std::endl;
  Response res = Docker::GetInstance().DELETE(getURL("", true));
  std::cout << res.code << std::endl;

  return res;
}

Response Container::inspect() {
  std::cout << "Inspecting: " << id << std::endl;
  Response res = Docker::GetInstance().GET(getURL("json", true));
  std::cout << res.code << std::endl;

  return res;
}

// TODO: actually implement query parameters
Response Container::logs(bool follow, bool stdout, bool stderr, int since, bool timestamps, int tail) {
  std::cout << "Fetching logs: " << id << std::endl;
  // Response res = POST("logs");
  Response res = Docker::GetInstance().GET(getURL("logs", true, "stdout=true&stderr=true"));
  std::cout << res.code << std::endl;

  return res;
}

void Container::signal(int signum) {
  if (kill(pid, signum) < 0) {
    perror("ERROR: failed to signal");
  }
}

void Container::setaffinity() {
  if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask) < 0) {
    perror("ERROR: failed to set affinity");
  }
}

void Container::getaffinity() {
  if (sched_getaffinity(pid, sizeof(cpu_set_t), &mask) < 0) {
    perror("ERROR: failed to set affinity");
  }
}

}
