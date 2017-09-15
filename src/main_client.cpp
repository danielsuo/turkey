#include "Client.h"
#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <thread>

#include <zmq.hpp>

using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting client" << std::endl;

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);
  std::cout << "Connecting to hello world server..." << std::endl;
  // socket.connect("ipc:///turkey-server");
  socket.connect("tcp://localhost:5555");

  for (int i = 0; i < 10; i++) {
    zmq::message_t request(5);
    memcpy (request.data(), "Hello", 5);
    std::cout << "Sending Hello " << i << "..." << std::endl;
    socket.send(request);

    zmq::message_t reply;
    socket.recv(&reply);
    std::cout << "Received " << reply.data() << std::endl;
  }
  return 0;
}
