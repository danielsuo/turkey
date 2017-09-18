#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <thread>
#include <sstream>

#include <zmq.hpp>

#include "fbs/fbs.h"

using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting client" << std::endl;

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_DEALER);
  std::stringstream ss;
  ss << getpid();
  socket.setsockopt(ZMQ_IDENTITY, ss.str().c_str(), ss.str().length());
  std::cout << "Connecting to hello world server from client " << getpid() << "..." << std::endl;
  // socket.connect("ipc:///turkey-server");
  socket.connect("tcp://localhost:5555");

  for (int i = 0; i < 10; i++) {
		flatbuffers::FlatBufferBuilder fbb;
		MessageBuilder builder(fbb);

		builder.add_type(MessageType_Start);
		builder.add_data(i);
    auto message = builder.Finish();
    fbb.Finish(message);
		
    zmq::message_t request(fbb.GetSize());
    memcpy (request.data(), fbb.GetBufferPointer(), fbb.GetSize());
    std::cout << "Sending Hello " << i << "..." << std::endl;
    socket.send(request);

    zmq::message_t reply;
    socket.recv(&reply);
    auto reply_message = GetMessage(reply.data());
    std::cout << "Received " << reply_message->data() << std::endl;
  }
  return 0;
}
