#pragma once

#include <sys/types.h>
#include <signal.h>
#include <sched.h>
#include <errno.h>
#include <sstream>
#include <fstream>
#include "Docker.h"
#include "json.hpp"

using json = nlohmann::json;

namespace docker {

class Container {
  json _args;
  int pid;
  cpu_set_t mask;
  std::string id;
  std::string getURL(std::string endpoint,
                     bool use_id = false,
                     std::string params = "");

public:
  Container(std::string args_path);
  Response attach();
  Response start();
  Response stop();
  Response remove();
  Response inspect();

  Response logs(bool follow     = false,
                bool stdout     = false,
                bool stderr     = false,
                int since       = 0,
                bool timestamps = false,
                int tail        = -1);

  void signal(int signum);
  void setaffinity();
  void getaffinity();
};

}
