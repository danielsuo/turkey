#include <iostream>
#include <thread>

#include "common/common.h"
#include "server/Server.h"

using namespace Turkey;

int main(int argc, char *argv[]) {
  std::cerr << "SERVER LOGS\n" << std::endl;

  // int port = TURKEY_SERVER_PORT;
  // if (argc > 1) {
  //   port = atoi(argv[1]);
  // }

  // Server::GetInstance().listen(port);

  // std::vector<std::string> args = {};
  //
  // std::stringstream ss;
  // ss << TURKEY_SERVER_IP_KEY;
  // ss << "=tcp://192.168.1.196:" << port;
  //
  // std::vector<std::string> envp = { ss.str() };

  // Server *server = new Server();
  // std::thread t(&Server::spawn, server, "/home/dsuo/turkey/build/apps/dummy/dummy",
  //     std::vector<std::string>(),
  //     std::vector<std::string>());

  std::thread t = Server::GetInstance().spawn("/home/dsuo/turkey/build/apps/dummy/dummy");
  t.join();

  // delete server;

  return 0;
}
