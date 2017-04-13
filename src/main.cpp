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
  while (true) {
    std::this_thread::sleep_for(1s);
    server.get();
  }
  return 0;
}
