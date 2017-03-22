#pragma once

#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <sstream>
#include <fstream>

#include "Docker.h"
#include "json.hpp"

using json = nlohmann::json;

namespace docker {

class Container {
  json _args;
  id_t _pid;
  cpu_set_t _mask;
  std::string _id;

  int _which;
  int _priority;

  std::string getURL(std::string endpoint,
                     bool use_id = false,
                     std::string params = "");

public:
  Container(std::string args_path);
  ~Container();

  Container(const Container&) = delete;
  Container& operator=(const Container&) = delete;
  Container(Container&&) = default;
  Container& operator=(Container&&) = default;

  Response attach();
  Response start();
  Response stop();
  Response remove();
  Response inspect();
  Response update(json data);

  Response logs(bool follow     = false,
                bool stdout     = false,
                bool stderr     = false,
                int since       = 0,
                bool timestamps = false,
                int tail        = -1);

  void signal(int signum);
  void setaffinity(cpu_set_t *mask);
  void getaffinity(cpu_set_t *mask);
  void setpriority(int priority);
  int getpriority();
};

}
