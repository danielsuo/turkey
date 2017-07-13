#include <iostream>

#include <atomic>
#include <chrono>
#include <thread>

#include <cstring>

// Signal
#include <signal.h>
#include <unistd.h>

std::atomic<bool> quit(false);
void got_signal(int) { quit.store(true); }

int main(int argc, char* argv[]) {
  // Set up the signal handler
  struct sigaction sa;
  std::memset(&sa, 0, sizeof(sa));
  sa.sa_handler = got_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  std::cout << "Starting server!" << std::endl;

  while (true) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);

    std::cout << "Polling..." << std::endl;

    if (quit.load()) {
      break;
    }
  }

  return 0;
}
