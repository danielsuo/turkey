#pragma once

#include "pthread/parsec_barrier.hpp"
#include <pthread.h>

#include "common/common.hpp"
#include "common/utils.hpp"

float pspeedy(Points* points, float z, long* kcenter, int pid,
              pthread_barrier_t* barrier) {
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  // my block
  long bsize = points->num / nproc;
  long k1 = bsize * pid;
  long k2 = k1 + bsize;
  if (pid == nproc - 1)
    k2 = points->num;

  static double totalcost;

  static bool open = false;
  static double* costs; // cost for each thread.
  static int i;

#ifdef ENABLE_THREADS
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
#endif

  /* create center at first point, send it to itself */
  for (int k = k1; k < k2; k++) {
    float distance = dist(points->p[k], points->p[0], points->dim);
    points->p[k].cost = distance * points->p[k].weight;
    points->p[k].assign = 0;
  }

  if (pid == 0) {
    *kcenter = 1;
    costs = (double*)malloc(sizeof(double) * nproc);
  }

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  if (pid !=
      0) { // we are not the master threads. we wait until a center is opened.
    while (1) {
#ifdef ENABLE_THREADS
      pthread_mutex_lock(&mutex);
      while (!open)
        pthread_cond_wait(&cond, &mutex);
      pthread_mutex_unlock(&mutex);
#endif
      if (i >= points->num)
        break;
      for (int k = k1; k < k2; k++) {
        float distance = dist(points->p[i], points->p[k], points->dim);
        if (distance * points->p[k].weight < points->p[k].cost) {
          points->p[k].cost = distance * points->p[k].weight;
          points->p[k].assign = i;
        }
      }
#ifdef ENABLE_THREADS
      pthread_barrier_wait(barrier);
      pthread_barrier_wait(barrier);
#endif
    }
  } else { // I am the master thread. I decide whether to open a center and
           // notify others if so.
    for (i = 1; i < points->num; i++) {
      bool to_open =
          ((float)lrand48() / (float)INT_MAX) < (points->p[i].cost / z);
      if (to_open) {
        (*kcenter)++;
#ifdef ENABLE_THREADS
        pthread_mutex_lock(&mutex);
#endif
        open = true;
#ifdef ENABLE_THREADS
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);
#endif
        for (int k = k1; k < k2; k++) {
          float distance = dist(points->p[i], points->p[k], points->dim);
          if (distance * points->p[k].weight < points->p[k].cost) {
            points->p[k].cost = distance * points->p[k].weight;
            points->p[k].assign = i;
          }
        }
#ifdef ENABLE_THREADS
        pthread_barrier_wait(barrier);
#endif
        open = false;
#ifdef ENABLE_THREADS
        pthread_barrier_wait(barrier);
#endif
      }
    }
#ifdef ENABLE_THREADS
    pthread_mutex_lock(&mutex);
#endif
    open = true;
#ifdef ENABLE_THREADS
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
#endif
  }
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  open = false;
  double mytotal = 0;
  for (int k = k1; k < k2; k++) {
    mytotal += points->p[k].cost;
  }
  costs[pid] = mytotal;
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  // aggregate costs from each thread
  if (pid == 0) {
    totalcost = z * (*kcenter);
    for (int i = 0; i < nproc; i++) {
      totalcost += costs[i];
    }
    free(costs);
  }
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  return (totalcost);
}

/* For a given point x, find the cost of the following operation:
 * -- open a facility at x if there isn't already one there,
 * -- for points y such that the assignment distance of y exceeds dist(y, x),
 *    make y a member of x,
 * -- for facilities y such that reassigning y and all its members to x
 *    would save cost, realize this closing and reassignment.
 *
 * If the cost of this operation is negative (i.e., if this entire operation
 * saves cost), perform this operation and return the amount of cost saved;
 * otherwise, do nothing.
 */

/* numcenters will be updated to reflect the new number of centers */
/* z is the facility cost, x is the number of this point in the array
   points */

double pgain(long x, Points* points, double z, long int* numcenters, int pid,
             pthread_barrier_t* barrier) {
//  printf("pgain pthread %d begin\n",pid);
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  // my block
  long bsize = points->num / nproc;
  long k1 = bsize * pid;
  long k2 = k1 + bsize;
  if (pid == nproc - 1)
    k2 = points->num;

  int i;
  int number_of_centers_to_close = 0;

  static double* work_mem;
  static double gl_cost_of_opening_x;
  static int gl_number_of_centers_to_close;

  // each thread takes a block of working_mem.
  int stride = *numcenters + 2;
  // make stride a multiple of CACHE_LINE
  int cl = CACHE_LINE / sizeof(double);
  if (stride % cl != 0) {
    stride = cl * (stride / cl + 1);
  }
  int K = stride - 2; // K==*numcenters

  // my own cost of opening x
  double cost_of_opening_x = 0;

  if (pid == 0) {
    work_mem = (double*)malloc(stride * (nproc + 1) * sizeof(double));
    gl_cost_of_opening_x = 0;
    gl_number_of_centers_to_close = 0;
  }

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  /*For each center, we have a *lower* field that indicates
    how much we will save by closing the center.
    Each thread has its own copy of the *lower* fields as an array.
    We first build a table to index the positions of the *lower* fields.
  */

  int count = 0;
  for (int i = k1; i < k2; i++) {
    if (is_center[i]) {
      center_table[i] = count++;
    }
  }
  work_mem[pid * stride] = count;

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  if (pid == 0) {
    int accum = 0;
    for (int p = 0; p < nproc; p++) {
      int tmp = (int)work_mem[p * stride];
      work_mem[p * stride] = accum;
      accum += tmp;
    }
  }

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  for (int i = k1; i < k2; i++) {
    if (is_center[i]) {
      center_table[i] += (int)work_mem[pid * stride];
    }
  }

  // now we finish building the table. clear the working memory.
  memset(switch_membership + k1, 0, (k2 - k1) * sizeof(bool));
  memset(work_mem + pid * stride, 0, stride * sizeof(double));
  if (pid == 0)
    memset(work_mem + nproc * stride, 0, stride * sizeof(double));

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  // my *lower* fields
  double* lower = &work_mem[pid * stride];
  // global *lower* fields
  double* gl_lower = &work_mem[nproc * stride];

  for (i = k1; i < k2; i++) {
    float x_cost =
        dist(points->p[i], points->p[x], points->dim) * points->p[i].weight;
    float current_cost = points->p[i].cost;

    if (x_cost < current_cost) {

      // point i would save cost just by switching to x
      // (note that i cannot be a median,
      // or else dist(p[i], p[x]) would be 0)

      switch_membership[i] = 1;
      cost_of_opening_x += x_cost - current_cost;

    } else {

      // cost of assigning i to x is at least current assignment cost of i

      // consider the savings that i's **current** median would realize
      // if we reassigned that median and all its members to x;
      // note we've already accounted for the fact that the median
      // would save z by closing; now we have to subtract from the savings
      // the extra cost of reassigning that median and its members
      int assign = points->p[i].assign;
      lower[center_table[assign]] += current_cost - x_cost;
    }
  }

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  // at this time, we can calculate the cost of opening a center
  // at x; if it is negative, we'll go through with opening it

  for (int i = k1; i < k2; i++) {
    if (is_center[i]) {
      double low = z;
      // aggregate from all threads
      for (int p = 0; p < nproc; p++) {
        low += work_mem[center_table[i] + p * stride];
      }
      gl_lower[center_table[i]] = low;
      if (low > 0) {
        // i is a median, and
        // if we were to open x (which we still may not) we'd close i

        // note, we'll ignore the following quantity unless we do open x
        ++number_of_centers_to_close;
        cost_of_opening_x -= low;
      }
    }
  }
  // use the rest of working memory to store the following
  work_mem[pid * stride + K] = number_of_centers_to_close;
  work_mem[pid * stride + K + 1] = cost_of_opening_x;

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  //  printf("thread %d cost complete\n",pid);

  if (pid == 0) {
    gl_cost_of_opening_x = z;
    // aggregate
    for (int p = 0; p < nproc; p++) {
      gl_number_of_centers_to_close += (int)work_mem[p * stride + K];
      gl_cost_of_opening_x += work_mem[p * stride + K + 1];
    }
  }
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  // Now, check whether opening x would save cost; if so, do it, and
  // otherwise do nothing

  if (gl_cost_of_opening_x < 0) {
    //  we'd save money by opening x; we'll do it
    for (int i = k1; i < k2; i++) {
      bool close_center = gl_lower[center_table[points->p[i].assign]] > 0;
      if (switch_membership[i] || close_center) {
        // Either i's median (which may be i itself) is closing,
        // or i is closer to x than to its current median
        points->p[i].cost =
            points->p[i].weight * dist(points->p[i], points->p[x], points->dim);
        points->p[i].assign = x;
      }
    }
    for (int i = k1; i < k2; i++) {
      if (is_center[i] && gl_lower[center_table[i]] > 0) {
        is_center[i] = false;
      }
    }
    if (x >= k1 && x < k2) {
      is_center[x] = true;
    }

    if (pid == 0) {
      *numcenters = *numcenters + 1 - gl_number_of_centers_to_close;
    }
  } else {
    if (pid == 0)
      gl_cost_of_opening_x = 0; // the value we'll return
  }
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  if (pid == 0) {
    free(work_mem);
    //    free(is_center);
    //    free(switch_membership);
    //    free(proc_cost_of_opening_x);
    //    free(proc_number_of_centers_to_close);
  }

  return -gl_cost_of_opening_x;
}

/* facility location on the points using local search */
/* z is the facility cost, returns the total cost and # of centers */
/* assumes we are seeded with a reasonable solution */
/* cost should represent this solution's cost */
/* halt if there is < e improvement after iter calls to gain */
/* feasible is an array of numfeasible points which may be centers */

float pFL(Points* points, int* feasible, int numfeasible, float z, long* k,
          double cost, long iter, float e, int pid,
          pthread_barrier_t* barrier) {
#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif
  long i;
  long x;
  double change;
  long numberOfPoints;

  change = cost;
  /* continue until we run iter iterations without improvement */
  /* stop instead if improvement is less than e */
  while (change / cost > 1.0 * e) {
    change = 0.0;
    numberOfPoints = points->num;
    /* randomize order in which centers are considered */

    if (pid == 0) {
      intshuffle(feasible, numfeasible);
    }
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif
    for (i = 0; i < iter; i++) {
      x = i % numfeasible;
      change += pgain(feasible[x], points, z, k, pid, barrier);
    }
    cost -= change;
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif
  }
  return (cost);
}

int selectfeasible_fast(Points* points, int** feasible, int kmin, int pid,
                        pthread_barrier_t* barrier) {
  int numfeasible = points->num;
  if (numfeasible > (ITER * kmin * log((double)kmin)))
    numfeasible = (int)(ITER * kmin * log((double)kmin));
  *feasible = (int*)malloc(numfeasible * sizeof(int));

  float* accumweight;
  float totalweight;

  /*
     Calcuate my block.
     For now this routine does not seem to be the bottleneck, so it is not
     parallelized.
     When necessary, this can be parallelized by setting k1 and k2 to
     proper values and calling this routine from all threads ( it is called only
     by thread 0 for now ).
     Note that when parallelized, the randomization might not be the same and it
     might
     not be difficult to measure the parallel speed-up for the whole program.
   */
  //  long bsize = numfeasible;
  long k1 = 0;
  long k2 = numfeasible;

  float w;
  int l, r, k;

  /* not many points, all will be feasible */
  if (numfeasible == points->num) {
    for (int i = k1; i < k2; i++)
      (*feasible)[i] = i;
    return numfeasible;
  }

  accumweight = (float*)malloc(sizeof(float) * points->num);

  accumweight[0] = points->p[0].weight;
  totalweight = 0;
  for (int i = 1; i < points->num; i++) {
    accumweight[i] = accumweight[i - 1] + points->p[i].weight;
  }
  totalweight = accumweight[points->num - 1];

  for (int i = k1; i < k2; i++) {
    w = (lrand48() / (float)INT_MAX) * totalweight;
    // binary search
    l = 0;
    r = points->num - 1;
    if (accumweight[0] > w) {
      (*feasible)[i] = 0;
      continue;
    }
    while (l + 1 < r) {
      k = (l + r) / 2;
      if (accumweight[k] > w) {
        r = k;
      } else {
        l = k;
      }
    }
    (*feasible)[i] = r;
  }

  free(accumweight);

  return numfeasible;
}

/* compute approximate kmedian on the points */
float pkmedian(Points* points, long kmin, long kmax, long* kfinal, int pid,
               pthread_barrier_t* barrier) {
  int i;
  double cost;
  double lastcost;
  double hiz, loz, z;

  static long k;
  static int* feasible;
  static int numfeasible;
  static double* hizs;

  if (pid == 0)
    hizs = (double*)calloc(nproc, sizeof(double));
  hiz = loz = 0.0;
  long numberOfPoints = points->num;
  long ptDimension = points->dim;

  // my block
  long bsize = points->num / nproc;
  long k1 = bsize * pid;
  long k2 = k1 + bsize;
  if (pid == nproc - 1)
    k2 = points->num;

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  double myhiz = 0;
  for (long kk = k1; kk < k2; kk++) {
    myhiz +=
        dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
  }
  hizs[pid] = myhiz;

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  for (int i = 0; i < nproc; i++) {
    hiz += hizs[i];
  }

  loz = 0.0;
  z = (hiz + loz) / 2.0;
  /* NEW: Check whether more centers than points! */
  if (points->num <= kmax) {
    /* just return all points as facilities */
    for (long kk = k1; kk < k2; kk++) {
      points->p[kk].assign = kk;
      points->p[kk].cost = 0;
    }
    cost = 0;
    if (pid == 0) {
      free(hizs);
      *kfinal = k;
    }
    return cost;
  }

  if (pid == 0)
    shuffle(points);
  cost = pspeedy(points, z, &k, pid, barrier);

  i = 0;
  /* give speedy SP chances to get at least kmin/2 facilities */
  while ((k < kmin) && (i < SP)) {
    cost = pspeedy(points, z, &k, pid, barrier);
    i++;
  }

  /* if still not enough facilities, assume z is too high */
  while (k < kmin) {
    if (i >= SP) {
      hiz = z;
      z = (hiz + loz) / 2.0;
      i = 0;
    }
    if (pid == 0)
      shuffle(points);
    cost = pspeedy(points, z, &k, pid, barrier);
    i++;
  }

  /* now we begin the binary search for real */
  /* must designate some points as feasible centers */
  /* this creates more consistancy between FL runs */
  /* helps to guarantee correct # of centers at the end */

  if (pid == 0) {
    numfeasible = selectfeasible_fast(points, &feasible, kmin, pid, barrier);
    for (int i = 0; i < points->num; i++) {
      is_center[points->p[i].assign] = true;
    }
  }

#ifdef ENABLE_THREADS
  pthread_barrier_wait(barrier);
#endif

  while (1) {
    /* first get a rough estimate on the FL solution */
    lastcost = cost;
    cost = pFL(points, feasible, numfeasible, z, &k, cost,
               (long)(ITER * kmax * log((double)kmax)), 0.1, pid, barrier);

    /* if number of centers seems good, try a more accurate FL */
    if (((k <= (1.1) * kmax) && (k >= (0.9) * kmin)) ||
        ((k <= kmax + 2) && (k >= kmin - 2))) {

      /* may need to run a little longer here before halting without
         improvement */
      cost = pFL(points, feasible, numfeasible, z, &k, cost,
                 (long)(ITER * kmax * log((double)kmax)), 0.001, pid, barrier);
    }

    if (k > kmax) {
      /* facilities too cheap */
      /* increase facility cost and up the cost accordingly */
      loz = z;
      z = (hiz + loz) / 2.0;
      cost += (z - loz) * k;
    }
    if (k < kmin) {
      /* facilities too expensive */
      /* decrease facility cost and reduce the cost accordingly */
      hiz = z;
      z = (hiz + loz) / 2.0;
      cost += (z - hiz) * k;
    }

    /* if k is good, return the result */
    /* if we're stuck, just give up and return what we have */
    if (((k <= kmax) && (k >= kmin)) || ((loz >= (0.999) * hiz))) {
      break;
    }
#ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
#endif
  }

  // clean up...
  if (pid == 0) {
    free(feasible);
    free(hizs);
    *kfinal = k;
  }

  return cost;
}

void* localSearchSub(void* arg_) {

  pkmedian_arg_t* arg = (pkmedian_arg_t*)arg_;
  pkmedian(arg->points, arg->kmin, arg->kmax, arg->kfinal, arg->pid,
           arg->barrier);

  return NULL;
}

void localSearch(Points* points, long kmin, long kmax, long* kfinal) {
  pthread_barrier_t barrier;
  pthread_t* threads = new pthread_t[nproc];
  pkmedian_arg_t* arg = new pkmedian_arg_t[nproc];

#ifdef ENABLE_THREADS
  pthread_barrier_init(&barrier, NULL, nproc);
#endif
  for (int i = 0; i < nproc; i++) {
    arg[i].points = points;
    arg[i].kmin = kmin;
    arg[i].kmax = kmax;
    arg[i].pid = i;
    arg[i].kfinal = kfinal;

    arg[i].barrier = &barrier;
#ifdef ENABLE_THREADS
    pthread_create(threads + i, NULL, localSearchSub, (void*)&arg[i]);
#else
    localSearchSub(&arg[0]);
#endif
  }

#ifdef ENABLE_THREADS
  for (int i = 0; i < nproc; i++) {
    pthread_join(threads[i], NULL);
  }
#endif

  delete[] threads;
  delete[] arg;
#ifdef ENABLE_THREADS
  pthread_barrier_destroy(&barrier);
#endif
}
