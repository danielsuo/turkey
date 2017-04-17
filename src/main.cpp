#include "ProcReader.h"
#include "Server.h"
#include <chrono>
#include <iostream>
#include <thread>
using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  Server server;
  std::cout << "Starting server" << std::endl;
  server.get();
  server.poll();
  return 0;
}
