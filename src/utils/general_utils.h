#ifndef TURKEY_GENERAL_UTILS_H
#define TURKEY_GENERAL_UTILS_H

#include <stdio.h>
#include <stdlib.h>

void pexit(char *msg) {
  perror(msg);
  exit(1);
}

#endif /* TURKEY_GENERAL_UTILS_H */
