#pragma once

#include "PStream/PStream.h"

class FileStream : public PStream {
public:
  FileStream(char* filename) {
    fp = fopen(filename, "rb");
    if (fp == NULL) {
      fprintf(stderr, "error opening file %s\n.", filename);
      exit(1);
    }
  }
  size_t read(float* dest, int dim, int num) {
    return std::fread(dest, sizeof(float) * dim, num, fp);
  }
  int ferror() { return std::ferror(fp); }
  int feof() { return std::feof(fp); }
  ~FileStream() {
    fprintf(stderr, "closing file stream\n");
    fclose(fp);
  }

private:
  FILE* fp;
};
