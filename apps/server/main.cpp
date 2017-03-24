#include <iostream>

#include "common.h"
#include "Server.h"

using namespace Turkey;

int main(int argc, char *argv[]) {
  std::cerr << "SERVER LOGS\n" << std::endl;
  Server::GetInstance().listen();

  pthread_exit(0);
  return 0;
}
