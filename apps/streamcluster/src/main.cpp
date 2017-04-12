/*
 * Copyright (C) 2008 Princeton University
 * All rights reserved.
 * Authors: Jia Deng, Gilberto Contreras
 *
 * streamcluster - Online clustering algorithm
 *
 */
#include <assert.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#include "PStream/FileStream.hpp"
#include "PStream/PStream.h"
#include "PStream/SimStream.hpp"
#include "common/Point.h"
#include "common/utils.hpp"

using namespace std;

#define MAXNAMESIZE 1024 // max filename length
#define SEED 1
/* increase this to reduce probability of random error */
/* increasing it also ups running time of "speedy" part of the code */
/* SP = 1 seems to be fine */
#define SP 1 // number of repetitions of speedy must be >=1

/* higher ITER --> more likely to get correct # of centers */
/* higher ITER also scales the running time almost linearly */
#define ITER 3 // iterate ITER* k log k times; ITER >= 1

#define CACHE_LINE 32 // cache line in byte

static bool* switch_membership; // whether to switch membership in pgain
static bool* is_center;         // whether a point is a center
static int* center_table;       // index table of centers

static int nproc; //# of threads

/********************************************/

#ifdef TBB_VERSION
#include "TBB/TBB.hpp"
#else // pthread / serial
#include "pthread/pthread.hpp"
#endif // TBB_VERSION

void streamCluster(PStream* stream, long kmin, long kmax, int dim,
                   long chunksize, long centersize, char* outfile) {

#ifdef TBB_VERSION
  float* block = (float*)memoryFloat.allocate(chunksize * dim * sizeof(float));
  float* centerBlock =
      (float*)memoryFloat.allocate(centersize * dim * sizeof(float));
  long* centerIDs = (long*)memoryLong.allocate(centersize * dim * sizeof(long));
#else
  float* block = (float*)malloc(chunksize * dim * sizeof(float));
  float* centerBlock = (float*)malloc(centersize * dim * sizeof(float));
  long* centerIDs = (long*)malloc(centersize * dim * sizeof(long));
#endif

  if (block == NULL) {
    fprintf(stderr, "not enough memory for a chunk!\n");
    exit(1);
  }

  Points points;
  points.dim = dim;
  points.num = chunksize;
  points.p =
#ifdef TBB_VERSION
      (Point*)memoryPoint.allocate(chunksize * sizeof(Point), NULL);
#else
      (Point*)malloc(chunksize * sizeof(Point));
#endif

  for (int i = 0; i < chunksize; i++) {
    points.p[i].coord = &block[i * dim];
  }

  Points centers;
  centers.dim = dim;
  centers.p =
#ifdef TBB_VERSION
      (Point*)memoryPoint.allocate(centersize * sizeof(Point), NULL);
#else
      (Point*)malloc(centersize * sizeof(Point));
#endif
  centers.num = 0;

  for (int i = 0; i < centersize; i++) {
    centers.p[i].coord = &centerBlock[i * dim];
    centers.p[i].weight = 1.0;
  }

  long IDoffset = 0;
  long kfinal;
  while (1) {

    size_t numRead = stream->read(block, dim, chunksize);
    fprintf(stderr, "read %d points\n", numRead);

    if (stream->ferror() ||
        numRead < (unsigned int)chunksize && !stream->feof()) {
      fprintf(stderr, "error reading data!\n");
      exit(1);
    }

    points.num = numRead;
    for (int i = 0; i < points.num; i++) {
      points.p[i].weight = 1.0;
    }

#ifdef TBB_VERSION
    switch_membership =
        (bool*)memoryBool.allocate(points.num * sizeof(bool), NULL);
    center_table = (int*)memoryInt.allocate(points.num * sizeof(int));
#else
    switch_membership = (bool*)malloc(points.num * sizeof(bool));
    center_table = (int*)malloc(points.num * sizeof(int));
#endif
    is_center = (bool*)calloc(points.num, sizeof(bool));

    // fprintf(stderr,"center_table = 0x%08x\n",(int)center_table);
    // fprintf(stderr,"is_center = 0x%08x\n",(int)is_center);

    localSearch(&points, kmin, kmax, &kfinal); // parallel

    // fprintf(stderr,"finish local search\n");
    contcenters(&points); /* sequential */
    if (kfinal + centers.num > centersize) {
      // here we don't handle the situation where # of centers gets too large.
      fprintf(stderr, "oops! no more space for centers\n");
      exit(1);
    }

    copycenters(&points, &centers, centerIDs, IDoffset); /* sequential */
    IDoffset += numRead;

    free(is_center);
#ifdef TBB_VERSION
    memoryBool.deallocate(switch_membership, sizeof(bool));
    memoryInt.deallocate(center_table, sizeof(int));
#else
    free(switch_membership);
    free(center_table);
#endif

    if (stream->feof()) {
      break;
    }
  }

// finally cluster all temp centers
#ifdef TBB_VERSION
  switch_membership = (bool*)memoryBool.allocate(centers.num * sizeof(bool));
  is_center = (bool*)calloc(centers.num, sizeof(bool));
  center_table = (int*)memoryInt.allocate(centers.num * sizeof(int));
#else
  switch_membership = (bool*)malloc(centers.num * sizeof(bool));
  is_center = (bool*)calloc(centers.num, sizeof(bool));
  center_table = (int*)malloc(centers.num * sizeof(int));
#endif

  localSearch(&centers, kmin, kmax, &kfinal); // parallel
  contcenters(&centers);
  outcenterIDs(&centers, centerIDs, outfile);
}

int main(int argc, char** argv) {
  char* outfilename = new char[MAXNAMESIZE];
  char* infilename = new char[MAXNAMESIZE];
  long kmin, kmax, n, chunksize, clustersize;
  int dim;

#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
  fprintf(stderr, "PARSEC Benchmark Suite Version "__PARSEC_XSTRING(
                      PARSEC_VERSION) "\n");
  fflush(NULL);
#else
  fprintf(stderr, "PARSEC Benchmark Suite\n");
  fflush(NULL);
#endif // PARSEC_VERSION
#ifdef ENABLE_PARSEC_HOOKS
  __parsec_bench_begin(__parsec_streamcluster);
#endif

  if (argc < 10) {
    fprintf(stderr,
            "usage: %s k1 k2 d n chunksize clustersize infile outfile nproc\n",
            argv[0]);
    fprintf(stderr, "  k1:          Min. number of centers allowed\n");
    fprintf(stderr, "  k2:          Max. number of centers allowed\n");
    fprintf(stderr, "  d:           Dimension of each data point\n");
    fprintf(stderr, "  n:           Number of data points\n");
    fprintf(stderr,
            "  chunksize:   Number of data points to handle per step\n");
    fprintf(stderr, "  clustersize: Maximum number of intermediate centers\n");
    fprintf(stderr, "  infile:      Input file (if n<=0)\n");
    fprintf(stderr, "  outfile:     Output file\n");
    fprintf(stderr, "  nproc:       Number of threads to use\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "if n > 0, points will be randomly generated instead of "
                    "reading from infile.\n");
    exit(1);
  }

  kmin = atoi(argv[1]);
  kmax = atoi(argv[2]);
  dim = atoi(argv[3]);
  n = atoi(argv[4]);
  chunksize = atoi(argv[5]);
  clustersize = atoi(argv[6]);
  strcpy(infilename, argv[7]);
  strcpy(outfilename, argv[8]);
  nproc = atoi(argv[9]);

#ifdef TBB_VERSION
  fprintf(stderr, "TBB version. Number of divisions: %d\n", NUM_DIVISIONS);
  tbb::task_scheduler_init init(nproc);
#endif

  srand48(SEED);
  PStream* stream;
  if (n > 0) {
    stream = new SimStream(n);
  } else {
    stream = new FileStream(infilename);
  }

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_begin();
#endif

  streamCluster(stream, kmin, kmax, dim, chunksize, clustersize, outfilename);

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_roi_end();
#endif

  delete stream;

#ifdef ENABLE_PARSEC_HOOKS
  __parsec_bench_end();
#endif

  return 0;
}
