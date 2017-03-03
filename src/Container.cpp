#include "Container.h"

namespace docker {

Container::Container(std::string args_path) {
  std::ifstream i(args_path);
  i >> _args;

  std::cout << _args.dump() << std::endl;

  Response res = Docker::GetInstance().POST("containers/create", _args.dump());

  if (res.code != 201) {
    fprintf(stderr, "ERROR: unable to create container");
    std::cerr << res.data["message"] << std::endl;
    exit(1);
  }

  std::cout << "Container successfully created!" << std::endl;

  id = res.data["Id"];
}

void Container::start() {
  std::cout << id << std::endl;
  Response res = Docker::GetInstance().POST("containers/" + id + "/start");
  std::cout << res.code << std::endl;
}

}
