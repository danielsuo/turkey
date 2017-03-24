#include <unistd.h>
#include <signal.h>

#include "common.h"
#include "Container.h"

using namespace docker;

void handler(int signum) { printf("Received signal %d\n", signum); }

int main(int argc, char *argv[]) {
  signal(SIGINT, handler);

  Container server("/home/dsuo/turkey/apps/server/docker.json");
  server.create();
  server.start();

  pause();

  Container dummy("/home/dsuo/turkey/apps/dummy/docker.json");
  std::stringstream ss;
  ss << "[\"" << TURKEY_SERVER_IP_KEY << "=" << server.getIP() << "\"]";
  std::cerr << ss.str() << std::endl;
  dummy.addArgs("Env", ss.str());
  dummy.create();
  dummy.start();

  pause();

  Response res = server.logs();
  std::cout << res.data << std::endl;
  res = dummy.logs();
  std::cout << res.data << std::endl;

  dummy.stop();
  dummy.remove();
  server.stop();
  server.remove();
}
