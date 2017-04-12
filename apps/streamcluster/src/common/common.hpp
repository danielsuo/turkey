#pragma once

struct pkmedian_arg_t {
  Points* points;
  long kmin;
  long kmax;
  long* kfinal;
  int pid;
  pthread_barrier_t* barrier;
};

/* compute the means for the k clusters */
int contcenters(Points* points) {
  long i, ii;
  float relweight;

  for (i = 0; i < points->num; i++) {
    /* compute relative weight of this point to the cluster */
    if (points->p[i].assign != i) {
      relweight = points->p[points->p[i].assign].weight + points->p[i].weight;
      relweight = points->p[i].weight / relweight;
      for (ii = 0; ii < points->dim; ii++) {
        points->p[points->p[i].assign].coord[ii] *= 1.0 - relweight;
        points->p[points->p[i].assign].coord[ii] +=
            points->p[i].coord[ii] * relweight;
      }
      points->p[points->p[i].assign].weight += points->p[i].weight;
    }
  }

  return 0;
}

/* copy centers from points to centers */
void copycenters(Points* points, Points* centers, long* centerIDs,
                 long offset) {
  long i;
  long k;

  bool* is_a_median = (bool*)calloc(points->num, sizeof(bool));

  /* mark the centers */
  for (i = 0; i < points->num; i++) {
    is_a_median[points->p[i].assign] = 1;
  }

  k = centers->num;

  /* count how many  */
  for (i = 0; i < points->num; i++) {
    if (is_a_median[i]) {
      memcpy(centers->p[k].coord, points->p[i].coord,
             points->dim * sizeof(float));
      centers->p[k].weight = points->p[i].weight;
      centerIDs[k] = i + offset;
      k++;
    }
  }

  centers->num = k;

  free(is_a_median);
}

void outcenterIDs(Points* centers, long* centerIDs, char* outfile) {
  FILE* fp = fopen(outfile, "w");
  if (fp == NULL) {
    fprintf(stderr, "error opening %s\n", outfile);
    exit(1);
  }
  int* is_a_median = (int*)calloc(sizeof(int), centers->num);
  for (int i = 0; i < centers->num; i++) {
    is_a_median[centers->p[i].assign] = 1;
  }

  for (int i = 0; i < centers->num; i++) {
    if (is_a_median[i]) {
      fprintf(fp, "%u\n", centerIDs[i]);
      fprintf(fp, "%lf\n", centers->p[i].weight);
      for (int k = 0; k < centers->dim; k++) {
        fprintf(fp, "%lf ", centers->p[i].coord[k]);
      }
      fprintf(fp, "\n\n");
    }
  }
  fclose(fp);
}
