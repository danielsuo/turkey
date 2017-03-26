#pragma once

#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <arpa/inet.h>

#include "common/common.h"
#include "docker/Docker.h"
#include "json.hpp"

using json = nlohmann::json;

namespace docker {

class Container {
  json _args;
  id_t _pid;
  cpu_set_t _mask;
  std::string _id;
  std::string _ip; // IP address in the usual 255.255.255.255 string format
  struct in_addr _in_addr; // IP address via inet_aton

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

  void addArgs(std::string key, std::string value);
  std::string getIP();

  Response create();
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
