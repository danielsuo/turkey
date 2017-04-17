#include "ProcReader.h"

#include <sstream>

static const std::string kProcStatPath = "/proc/stat";
namespace Turkey {

ProcReader::ProcReader() {
  istream_ = std::ifstream(kProcStatPath, std::ios::in);
  if (!istream_.is_open()) {
    throw std::runtime_error("Failed to open /proc/stat");
  }
}

size_t ProcReader::getRunnableThreads() {
  std::string line;
  while (std::getline(istream_, line)) {
    std::istringstream lineStream(line);
    std::string category;
    lineStream >> category;
    if (category == "procs_running") {
      size_t r;
      lineStream >> r;
      return r;
    }
  }
}
}
