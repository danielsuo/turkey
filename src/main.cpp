#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "cgroup/cgroup.h"
#include "utils/general.h"

void handler(int signum) { printf("Received signal %d and continuing\n", signum); }

int main(int argc, char *argv[]) {
  signal(SIGINT, handler);

  int ret = cgroup_init();

  struct cgroup *g = cgroup_new_cgroup("dummy");
  struct cgroup_controller *gctl = cgroup_add_controller(g, "cpu");

  if (cgroup_create_cgroup(g, 0) != 0) {
    cgroup_delete_cgroup(g, 1);
    cgroup_pexit(g, "Failed to create group in kernel");
  }

  pause();

  if (cgroup_attach_task(g) != 0) {
    cgroup_pexit(g, "Failed to attach task");
  }

  if (cgroup_add_value_int64(gctl, "cpu.shares", 512) != 0) {
    cgroup_pexit(g, "Failed to modify shares");
  }

  if (cgroup_modify_cgroup(g) != 0) {
    cgroup_pexit(g, "Failed to propagate changes to cgroup");
  }

  pause();

  cgroup_cleanup(g);

  return 0;
}

// #include <unistd.h>
// #include <signal.h>
//
// #include "common.h"
// #include "Container.h"
//
// using namespace docker;
//
// void handler(int signum) { printf("Received signal %d\n", signum); }
//
// int main(int argc, char *argv[]) {
//   signal(SIGINT, handler);
//
//   Container server("/home/dsuo/turkey/apps/server/docker.json");
//   server.create();
//   server.start();
//
//   pause();
//
//   Container dummy("/home/dsuo/turkey/apps/dummy/docker.json");
//   std::stringstream ss;
//
//   ss << "[\"";
//   ss << TURKEY_SERVER_IP_KEY;
//   ss << "=tcp://" << server.getIP() << ":" << TURKEY_SERVER_PORT;
//   ss << "\"]";
//   std::cerr << ss.str() << std::endl;
//   dummy.addArgs("Env", ss.str());
//   dummy.create();
//   dummy.start();
//
//   pause();
//
//   Response res = server.logs();
//   std::cout << res.data << std::endl;
//   res = dummy.logs();
//   std::cout << res.data << std::endl;
//
//   dummy.stop();
//   dummy.remove();
//   server.stop();
//   server.remove();
// }
