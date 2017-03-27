#include "cgroup.h"

void cgroup_cleanup(struct cgroup *g) {
  cgroup_delete_cgroup(g, 1);
  cgroup_free_controllers(g);
  cgroup_free(&g);
}

void cgroup_pexit(struct cgroup *g, char *msg) {
  cgroup_cleanup(g);
  pexit(msg);
}
