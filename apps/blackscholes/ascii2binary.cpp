#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define fptype float

typedef struct OptionData_ {
  fptype s;        // spot price
  fptype strike;   // strike price
  fptype r;        // risk-free interest rate
  fptype divq;     // dividend rate
  fptype v;        // volatility
  fptype t;        // time to maturity or option expiration in years
                   //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)
  char OptionType; // Option type.  "P"=PUT, "C"=CALL
  fptype divs;     // dividend vals (not used in this test)
  fptype DGrefval; // DerivaGem Reference Value
} OptionData;

void ascii2binary(const char* input_file, const char *output_file);
void binary2ascii(const char* input_file, const char *output_file);

int main(int argc, char* argv[]) {
  for (int i = 0; i < argc; i++) {
    cout << argv[i] << endl;
  }

  if (argc < 3) {
    exit(-1);
  }

  // ascii2binary(argv[1], argv[2]);
  binary2ascii(argv[2], argv[1]);
}

void ascii2binary(const char* input_file, const char* output_file) {
  FILE* file;
  int rv;
  OptionData* data;
  int numOptions;

  file = fopen(input_file, "r");
  if (file == NULL) {
    printf("ERROR: Unable to open file %s.\n", input_file);
    exit(1);
  }

  rv = fscanf(file, "%i", &numOptions);
  if (rv != 1) {
    printf("ERROR: Unable to read from file %s.\n", input_file);
    fclose(file);
    exit(1);
  }

  data = (OptionData*)malloc(numOptions * sizeof(OptionData));
  for (int i = 0; i < numOptions; ++i) {
    rv = fscanf(file, "%f %f %f %f %f %f %c %f %f", &data[i].s,
                &data[i].strike, &data[i].r, &data[i].divq,
                &data[i].v, &data[i].t, &data[i].OptionType,
                &data[i].divs, &data[i].DGrefval);
    if (rv != 9) {
      printf("ERROR: Unable to read from file %s.\n", input_file);
      fclose(file);
      exit(1);
    }
  }

  rv = fclose(file);
  if (rv != 0) {
    printf("ERROR: Unable to close file %s.\n", input_file);
    exit(1);
  }

  file = fopen(output_file, "wb");
  if (file == NULL) {
    printf("ERROR: Unable to open file %s.\n", output_file);
    exit(1);
  }

  fwrite(&numOptions, sizeof(numOptions), 1, file);
  fwrite(data, sizeof(OptionData), numOptions, file);

  rv = fclose(file);
  if (rv != 0) {
    printf("ERROR: Unable to close file %s.\n", output_file);
    exit(1);
  }

  free(data);
}

void binary2ascii(const char* input_file, const char* output_file) {
  FILE* file;
  int rv;
  OptionData* data;
  int numOptions;

  file = fopen(input_file, "rb");
  if (file == NULL) {
    printf("ERROR: Unable to open file %s.\n", input_file);
    exit(1);
  }

  fread(&numOptions, sizeof(numOptions), 1, file);

  cerr << "Got options: " << numOptions << endl;

  data = (OptionData*)malloc(numOptions * sizeof(OptionData));
  fread(data, sizeof(OptionData), numOptions, file);

  // for (int i = 0; i < numOptions; i++) {
    // fprintf(stderr, "%f %f %f %f %f %f %c %f %f\n", data[i].s,
    //             data[i].strike, data[i].r, data[i].divq,
    //             data[i].v, data[i].t, data[i].OptionType,
    //             data[i].divs, data[i].DGrefval);
  // }

  free(data);
}
