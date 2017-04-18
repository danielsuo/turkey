#include "Pool.h"
#include <chrono>
#include <iostream>
#include <thread>
using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting pool" << std::endl;
  DynamicThreadPoolExecutor pool(1);

  auto func = []() {
    while (true) {
      LOG(INFO) << "Lol...";
      std::this_thread::sleep_for(1s);
    }
  };

  pool.add(func);
  pool.join();

  return 0;
}
