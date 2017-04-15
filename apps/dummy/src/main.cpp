#include <assert.h>
#include <fstream>

int main(int argc, char* argv[]) {
  std::ifstream fin(argv[1]);
  assert(fin.is_open());

  unsigned x, y, z;
  fin >> x >> y >> z;
  fprintf(stderr, "%s, %d, %d, %d\n", argv[1], x, y, z);

  return 0;
}
