#ifndef TURKEY_GENERAL_UTILS_H
#define TURKEY_GENERAL_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(Turkey, x)

void pexit(char *msg) {
  perror(msg);
  exit(1);
}

#ifdef __cplusplus
}
#endif

#endif /* TURKEY_GENERAL_UTILS_H */
