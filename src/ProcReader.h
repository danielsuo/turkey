#pragma once
#include <fstream>
namespace Turkey {

class ProcReader {
public:
  ProcReader();

  int getRunnableThreads();

private:
  std::ifstream istream_;
};
}
