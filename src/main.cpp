#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <iostream>

#include "Docker.h"
#include "Container.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace docker;

void test_json() {
  json j2 = {
    {"pi", 3.141},
    {"happy", true},
    {"name", "Niels"},
    {"nothing", nullptr},
    {"answer", {
              {"everything", 42}
           }},
    {"list", {1, 0, 2}},
    {"object", {
              {"currency", "USD"},
                 {"value", 42.99}
           }}
  };
}

#define SRV_FLAG  "producer"
#define SHM_PATH "/dev/shm"
#define SHMSZ 27

void handler(int signum) {
  printf("Received signal %d\n", signum);
}

void producer() {
  printf("producer!\n");

  signal(SIGINT, handler);

  char c;
  int shmid;
  key_t key;
  unsigned char *shm, *s;

  /*
   * We'll name our shared memory segment
   * "5678".
   */
  key = ftok(SHM_PATH, 1);

  /*
   * Create the segment.
   */
  if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  /*
   * Now we attach the segment to our data space.
   */

  shm = (unsigned char *)shmat(shmid, NULL, 0);
  if (shm == (unsigned char *)-1) {
    perror("shmat");
    exit(1);
  }

  /*
   * Now put some things into the memory for the
   * other process to read.
   */
  s = shm;

  for (c = 'a'; c <= 'z'; c++)
    *s++ = c;
  *s = NULL;

  /*
   * Finally, we wait until the other process
   * changes the first character of our memory
   * to '*', indicating that it has read what
   * we put there.
   */
  while (*shm != '*')
    sleep(1);

  exit(0);
}

void consumer() {
  printf("consumer!\n");

  int shmid;
  key_t key;
  unsigned char *shm, *s;

  /*
   * We need to get the segment named
   * "5678", created by the server.
   */
  key = ftok(SHM_PATH, 1);

  /*
   * Locate the segment.
   */
  if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
    perror("shmget");
    exit(1);
  }

  /*
   * Now we attach the segment to our data space.
   */
  shm = (unsigned char *)shmat(shmid, NULL, 0);
  if (shm == (unsigned char *)-1) {
    perror("shmat");
    exit(1);
  }

  /*
   * Now read what the server put in the memory.
   */
  for (s = shm; *s != NULL; s++)
    putchar(*s);
  putchar('\n');

  /*
   * Finally, change the first character of the
   * segment to '*', indicating we have read
   * the segment.
   */
  *shm = '*';

  exit(0);
}

int main(int argc, char *argv[]) {
  int ncore = std::thread::hardware_concurrency();
  std::cout << "Number of cores: " << ncore << std::endl;

  Response res = Docker::GetInstance().GET("images/json");
  std::cout << res.data.dump() << std::endl;

  // res = Docker::GetInstance().POST("containers/create",
  //   "{\"Image\": \"alpine\", \"Cmd\": [\"echo\", \"hello world\"]}");
  // std::cout << res.data.dump() << std::endl;

  Container container("/home/ubuntu/turkey/jobs/client.json");
  container.start();

  // if (argc < 2 || 0 == strncmp(argv[1], SRV_FLAG, strlen(SRV_FLAG))) {
  //     producer();
  // } else {
  //     consumer();
  // }
  return 0;
}
