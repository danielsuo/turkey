#include <iostream>

#include <zmq.hpp>

#include "common.h"
#include "Server.h"

using namespace Turkey;

int main(int argc, char *argv[]) {
  std::cerr << "SERVER LOGS\n" << std::endl;
  Server::GetInstance().listen();

  zmq::context_t context = zmq::context_t(1);
  zmq::socket_t socket = zmq::socket_t(context, ZMQ_PULL);
  socket.bind("tcp://0.0.0.0:21217");

  while (1) {
    zmq::message_t receiveMessage;
    socket.recv(&receiveMessage);
    fprintf(stderr, "%s\n", receiveMessage.data());
  }

  pthread_exit(0);
  return 0;
}
