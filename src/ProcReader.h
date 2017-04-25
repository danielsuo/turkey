#pragma once
#include <fstream>
namespace Turkey {

class ProcReader {
public:
  ProcReader();

  size_t getRunnableThreads();

private:
  std::ifstream istream_;
};
}
