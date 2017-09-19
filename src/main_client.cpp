#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <thread>

#include "Client.h"

using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting client" << std::endl;

  Client client("tcp://localhost:5555");

  for (int i = 0; i < 10; i++) {
    std::cout << "Sending Hello " << i << "..." << std::endl;
    client.sendMessage(MessageType_Start, i);
    auto msg = client.recvAndProcessMessage();
    // LOG(INFO) << msg->data();
    
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
  }
  return 0;
}
