#include <iostream>

#include "common.h"
#include "Server.h"

using namespace Turkey;

int main(int argc, char *argv[]) {
  std::cerr << "SERVER LOGS\n" << std::endl;

  if (argc > 1) {
    Server::GetInstance().listen(atoi(argv[1]));
  } else {
    Server::GetInstance().listen();
  }

  pthread_exit(0);
  return 0;
}
