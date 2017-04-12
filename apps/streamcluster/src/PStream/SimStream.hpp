#pragma once

#include "PStream/PStream.h"

// synthetic stream
class SimStream : public PStream {
public:
  SimStream(long n_) { n = n_; }
  size_t read(float* dest, int dim, int num) {
    size_t count = 0;
    for (int i = 0; i < num && n > 0; i++) {
      for (int k = 0; k < dim; k++) {
        dest[i * dim + k] = lrand48() / (float)INT_MAX;
      }
      n--;
      count++;
    }
    return count;
  }
  int ferror() { return 0; }
  int feof() { return n <= 0; }
  ~SimStream() {}

private:
  long n;
};
