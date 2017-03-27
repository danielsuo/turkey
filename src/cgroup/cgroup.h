#pragma once

#include <libcgroup.h>

#include "utils/general.h"

#ifdef __cplusplus
extern "C" {
#endif

void cgroup_cleanup(struct cgroup *g);
void cgroup_pexit(struct cgroup *g, char *msg);

#ifdef __cplusplus
}
#endif
