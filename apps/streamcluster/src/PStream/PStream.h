#pragma once

class PStream {
public:
  virtual size_t read(float* dest, int dim, int num) = 0;
  virtual int ferror() = 0;
  virtual int feof() = 0;
  virtual ~PStream() {}
};
