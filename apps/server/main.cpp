#include <iostream>

#include "common/common.h"
#include "server/Server.h"

using namespace Turkey;

int main(int argc, char *argv[]) {
  std::cerr << "SERVER LOGS\n" << std::endl;

  // if (argc > 1) {
  //   Server::GetInstance().listen(atoi(argv[1]));
  // } else {
  //   Server::GetInstance().listen();
  // }

  Server::GetInstance().spawn("/bin/ls");

  return 0;
}
