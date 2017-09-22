#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <thread>

// #include "Pool.h"
#include "Client.h"

using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting client" << std::endl;

  Client client;

  client.start();
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(2s);
  client.stop();
  // std::this_thread::sleep_for(1s);
  return 0;
}
