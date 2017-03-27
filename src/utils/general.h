#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void pexit(char *msg);
char *tsprintf(const char *format, ...);

#ifdef __cplusplus
}
#endif
