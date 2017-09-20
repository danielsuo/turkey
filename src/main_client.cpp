#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <thread>

#include "Pool.h"
// #include "Client.h"

using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting client" << std::endl;

  DynamicThreadPool dtp(16);
  dtp.start();

  // Client client("tcp://localhost:5555",
  // [](const Message* msg) { LOG(INFO) << msg->data(); });

  // for (int i = 0; i < 10; i++) {
  // std::cout << "Sending Hello " << i << "..." << std::endl;
  // client.sendMessage(MessageType_Start, i);
  // client.recvAndProcessMessage();
  // LOG(INFO) << msg->data();

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  dtp.stop();
  // }
  return 0;
}
