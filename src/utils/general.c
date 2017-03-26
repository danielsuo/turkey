#include "general.h"

void pexit(char *msg) {
  perror(msg);
  exit(1);
}

char *tsprintf(const char *format, ...) {
  size_t len;
  char *str;

  va_list args;

  va_start(args, format);
  if ((len = vsnprintf(NULL, 0, format, args)) < 0) {
    fprintf(stderr, "failed to get len\n");
    return NULL;
  }

  va_end(args);
  va_start(args, format);

  if ((str = (char *)malloc(++len)) == NULL) {
    fprintf(stderr, "failed to alloc\n");
    return NULL;
  }

  if (vsnprintf(str, len, format, args) < 0) {
    fprintf(stderr, "failed to copy\n");
    return NULL;
  }

  va_end(args);

  return str;
}
