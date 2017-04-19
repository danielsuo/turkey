#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define fptype float

typedef struct element_ {
  int32_t id;
  int32_t type;
  int32_t n[5];
} element;

void ascii2binary(const char* input_file, const char *output_file);
void binary2ascii(const char* input_file, const char *output_file);

int main(int argc, char* argv[]) {
  for (int i = 0; i < argc; i++) {
    cout << argv[i] << endl;
  }

  if (argc < 3) {
    exit(-1);
  }

  ascii2binary(argv[1], argv[2]);
  binary2ascii(argv[2], argv[1]);
}

int string2int(std::string str) {
  int len = str.size();

  int id = 0;
  for (int i = 0; i < len; i++) {
    id = id * 26 + (str[i] - 'a' + 1);
  }
  return id;
}

void ascii2binary(const char* input_file, const char* output_file) {
  FILE* file;
  element* data;
  unsigned numElements, max_x, max_y;
  std::string end;

  ifstream fin(input_file);
  // assert(fin.is_open());

  fin >> numElements >> max_x >> max_y;

  fprintf(stderr, "%d %d %d\n", numElements, max_x, max_y);

  // rv = fscanf(file, "%i", &numElements);
  // if (rv != 1) {
  //   printf("ERROR: Unable to read from file %s.\n", input_file);
  //   fclose(file);
  //   exit(1);
  // }

  data = (element*)malloc(numElements * sizeof(element));
  if (data == NULL) {
    printf("ERROR: failed to allocate memory.");
  }
  std::string raw;
  for (int i = 0; i < numElements; ++i) {
    fin >> raw;
    data[i].id = string2int(raw);
    fin >> data[i].type;

    for (int j = 0; j < 5; j++) {
      fin >> raw;
      data[i].n[j] = string2int(raw);
    }
    fin >> end;
    // fprintf(stderr, "%d %d %d %d %d %d %d\n", data[i].id, data[i].type, data[i].n[0], data[i].n[1], data[i].n[2], data[i].n[3], data[i].n[4]);
  }

  // int rv = fclose(file);
  // if (rv != 0) {
  //   printf("ERROR: Unable to close file %s.\n", input_file);
  //   exit(1);
  // }

  file = fopen(output_file, "wb");
  if (file == NULL) {
    printf("ERROR: Unable to open file %s .\n", output_file);
    exit(1);
  }

  fwrite(&numElements, sizeof(unsigned), 1, file);
  fwrite(&max_x, sizeof(unsigned), 1, file);
  fwrite(&max_y, sizeof(unsigned), 1, file);
  fwrite(data, sizeof(element), numElements, file);

  int rv = fclose(file);
  if (rv != 0) {
    printf("ERROR: Unable to close file %s.\n", output_file);
    exit(1);
  }

  free(data);
}

void binary2ascii(const char* input_file, const char* output_file) {
  FILE* file;
  int rv;
  element* data;
  int numElements, max_x, max_y;

  file = fopen(input_file, "rb");
  if (file == NULL) {
    printf("ERROR: Unable to open file %s.\n", input_file);
    exit(1);
  }

  fread(&numElements, sizeof(unsigned), 1, file);
  fread(&max_x, sizeof(unsigned), 1, file);
  fread(&max_y, sizeof(unsigned), 1, file);

  cerr << "Got elements: " << numElements << endl;

  data = (element*)malloc(numElements * sizeof(element));
  fread(data, sizeof(element), numElements, file);

  for (int i = 0; i < numElements; i++) {
    fprintf(stderr, "%d %d %d %d %d %d %d\n", data[i].id, data[i].type, data[i].n[0], data[i].n[1], data[i].n[2], data[i].n[3], data[i].n[4]);
  }

  fclose(file);
  free(data);
}
