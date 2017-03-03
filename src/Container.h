#pragma once

#include <fstream>
#include "Docker.h"
#include "json.hpp"

using json = nlohmann::json;

namespace docker {

class Container {
  json _args;
  std::string id;
public:
  Container(std::string args_path);
  void start();
};

}
