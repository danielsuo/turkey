#pragma once

#include "common/common.hpp"
#include "common/utils.hpp"

#define TBB_STEALER (tbb::task_scheduler_init::occ_stealer)
#define NUM_DIVISIONS (nproc)
#include "tbb/blocked_range.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/task_scheduler_init.h"

using namespace tbb;

tbb::cache_aligned_allocator<float> memoryFloat;
tbb::cache_aligned_allocator<Point> memoryPoint;
tbb::cache_aligned_allocator<long> memoryLong;
tbb::cache_aligned_allocator<int> memoryInt;
tbb::cache_aligned_allocator<bool> memoryBool;

struct HizReduction {
private:
  double hiz;

public:
  Points* points;
  HizReduction(Points* points_) : hiz(0), points(points_) {}
  HizReduction(HizReduction& d, tbb::split) {
    hiz = 0;
    points = d.points;
  }

  void operator()(const tbb::blocked_range<int>& range) {
    double myhiz = 0;
    long ptDimension = points->dim;
    int begin = range.begin();
    int end = range.end();

    for (int kk = begin; kk != end; kk++) {
      myhiz +=
          dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }
    hiz += myhiz;
  }

  void join(HizReduction& d) {
    hiz += d.getHiz(); /*fprintf(stderr,"reducing: %lf\n",hiz);*/
  }
  double getHiz() { return hiz; }
};

struct CenterCreate {
  Points* points;
  CenterCreate(Points* p) : points(p) {}
  void operator()(const tbb::blocked_range<int>& range) const {
    int begin = range.begin();
    int end = range.end();

    for (int k = begin; k != end; k++) {
      float distance = dist(points->p[k], points->p[0], points->dim);
      points->p[k].cost = distance * points->p[k].weight;
      points->p[k].assign = 0;
    }
  }
};

struct CenterOpen {
private:
  double total_cost;

public:
  Points* points;
  int i;
  int type; /*type=0: compute. type=1: reduction */
  CenterOpen(Points* p) : points(p), total_cost(0), type(0) {}
  CenterOpen(CenterOpen& rhs, tbb::split) {
    total_cost = 0;
    points = rhs.points;
    i = rhs.i;
    type = rhs.type;
  }

  void operator()(const tbb::blocked_range<int>& range) {
    int begin = range.begin();
    int end = range.end();

    if (type) {
      double local_total = 0.0;
      for (int k = begin; k != end; k++)
        local_total += points->p[k].cost;
      total_cost += local_total;
    } else {
      for (int k = begin; k != end; k++) {
        float distance = dist(points->p[i], points->p[k], points->dim);
        if (i && distance * points->p[k].weight < points->p[k].cost) {
          points->p[k].cost = distance * points->p[k].weight;
          points->p[k].assign = i;
        }
      }
    }
  }

  void join(CenterOpen& lhs) { total_cost += lhs.getTotalCost(); }
  double getTotalCost() { return total_cost; }
};

class CenterTableCount : public tbb::task {
private:
  Points* points;
  double* work_mem;
  int stride;
  int pid;

public:
  CenterTableCount(int id, int s, Points* p, double* mem)
      : pid(id), stride(s), points(p), work_mem(mem) {}

  task* execute() {
    int count = 0;
    long bsize = points->num / ((NUM_DIVISIONS));
    long k1 = bsize * pid;
    long k2 = k1 + bsize;

    if (pid == (NUM_DIVISIONS)-1)
      k2 = points->num;

    /* fprintf(stderr,"\t[CenterTableCount]: pid=%d stride=%d from %d to %d\n",
       pid, stride, k1, k2); */

    for (int i = k1; i < k2; i++) {
      if (is_center[i]) {
        center_table[i] = count++;
      }
    }

    work_mem[pid * stride] = count;
    // fprintf(stderr,"PID %d done!\n",pid);
    return NULL;
  }
};

class CenterTableCountTask : public tbb::task {
  int is_continuation;
  Points* points;
  double* work_mem;
  int stride;

public:
  CenterTableCountTask(int s, Points* p, double* mem)
      : stride(s), points(p), work_mem(mem), is_continuation(0) {}

  task* execute() {
    tbb::task_list list;
    int p;

    if (!is_continuation) {
      recycle_as_continuation();
      set_ref_count(NUM_DIVISIONS);

      for (p = 1; p < (NUM_DIVISIONS); p++)
        list.push_back(*new (allocate_child())
                           CenterTableCount(p, stride, points, work_mem));
      CenterTableCount& me =
          *new (allocate_child()) CenterTableCount(0, stride, points, work_mem);
      spawn(list);
      is_continuation = 1;

      return &me;

    } else {
      /* continuation part */
      int accum = 0;
      for (int p = 0; p < (NUM_DIVISIONS); p++) {
        int tmp = (int)work_mem[p * stride];
        work_mem[p * stride] = accum;
        accum += tmp;
      }
      // fprintf(stderr,"Accum = %d\n",accum);
      return NULL;
    }
  }
};

class FixCenter : public tbb::task {
  Points* points;
  double* work_mem;
  int pid;
  int stride;

public:
  FixCenter(int id, int s, Points* p, double* mem)
      : pid(id), stride(s), points(p), work_mem(mem) {}
  task* execute() {
#ifdef SERIAL_FIXCENTER
    long k1 = 0;
    long k2 = points->num;
#else
    long bsize = points->num / ((NUM_DIVISIONS));
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    if (pid == (NUM_DIVISIONS)-1)
      k2 = points->num;
#endif
    /*fprintf(stderr,"\t[FixCenter]: pid=%d stride=%d from %d to %d
      is_center=0x%08x\n",
      pid, stride, k1, k2,(int)is_center);  */

    for (int i = k1; i < k2; i++) {
      if (is_center[i]) {
        center_table[i] += (int)work_mem[pid * stride];
        // fprintf(stderr,"\tcenter_table[%d] = %d\n",i,center_table[i]);
      }
    }
    // fprintf(stderr,"PID %d done!\n",pid);
    return NULL;
  }
};

class FixCenterTask : public tbb::task {
  bool is_continuation;
  Points* points;
  double* work_mem;
  int stride;

public:
  FixCenterTask(int s, Points* p, double* mem)
      : stride(s), points(p), work_mem(mem), is_continuation(false) {}

  task* execute() {
    tbb::task_list list;
    int p;
    if (!is_continuation) {
      recycle_as_continuation();
      set_ref_count(NUM_DIVISIONS);
      for (p = 1; p < (NUM_DIVISIONS); p++)
        list.push_back(*new (allocate_child())
                           FixCenter(p, stride, points, work_mem));
      spawn(list);
      FixCenter& me =
          *new (allocate_child()) FixCenter(0, stride, points, work_mem);
      is_continuation = true;
      return &me;
    } else {
      /* coninuation */
      return NULL;
    }
  }
};

class LowerCost : public tbb::task {
  Points* points;
  double* work_mem;
  long x;
  int K;
  int pid;
  int stride;

public:
  LowerCost(int id, int s, Points* p, long x_, double* mem, int k)
      : pid(id), stride(s), points(p), work_mem(mem), K(k), x(x_) {}
  task* execute() {

    // my *lower* fields
    double* lower = &work_mem[pid * stride];
    double local_cost_of_opening_x = 0;
    long bsize =
        points->num / ((NUM_DIVISIONS)); // points->num/1;//((NUM_DIVISIONS));
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    int i;

    if (pid == (NUM_DIVISIONS)-1)
      k2 = points->num;

    /*fprintf(stderr,"\t[LowerCost]: pid=%d stride=%d from %d to %d\n",
      pid, stride, k1, k2);  */

    double* cost_of_opening_x = &work_mem[pid * stride + K + 1];

    for (i = k1; i < k2; i++) {
      float x_cost =
          dist(points->p[i], points->p[x], points->dim) * points->p[i].weight;
      float current_cost = points->p[i].cost;

      // fprintf(stderr,"\t (x_cost=%lf < current_cost=%lf)\n",x_cost,
      // current_cost);
      if (x_cost < current_cost) {

        // point i would save cost just by switching to x
        // (note that i cannot be a median,
        // or else dist(p[i], p[x]) would be 0)

        switch_membership[i] = 1;
        local_cost_of_opening_x += x_cost - current_cost;

      } else {

        // cost of assigning i to x is at least current assignment cost of i

        // consider the savings that i's **current** median would realize
        // if we reassigned that median and all its members to x;
        // note we've already accounted for the fact that the median
        // would save z by closing; now we have to subtract from the savings
        // the extra cost of reassigning that median and its members
        int assign = points->p[i].assign;
        lower[center_table[assign]] += current_cost - x_cost;
        // fprintf(stderr,"Lower[%d]=%lf\n",center_table[assign],
        // lower[center_table[assign]]);
      }
    }

    *cost_of_opening_x = local_cost_of_opening_x;
    return NULL;
  }
};

class LowerCostTask : public tbb::task {
  bool is_continuation;
  Points* points;
  double* work_mem;
  int K;
  long x;
  int stride;

public:
  LowerCostTask(int s, Points* p, long x_, double* mem, int k)
      : stride(s), points(p), work_mem(mem), K(k), x(x_),
        is_continuation(false) {}

  task* execute() {
    tbb::task_list list;
    int p;
    if (!is_continuation) {
      recycle_as_continuation();
      set_ref_count(NUM_DIVISIONS);
      for (p = 1; p < (NUM_DIVISIONS); p++)
        list.push_back(*new (allocate_child())
                           LowerCost(p, stride, points, x, work_mem, K));
      spawn(list);
      LowerCost& me =
          *new (allocate_child()) LowerCost(0, stride, points, x, work_mem, K);
      is_continuation = true;
      return &me;
    } else {
      /* continuation */
      return NULL;
    }
  }
};

class CenterClose : public tbb::task {
  Points* points;
  double* work_mem;
  double* number_of_centers_to_close;
  double z;
  int pid, stride;
  int K;

public:
  CenterClose(int id, int s, Points* p, double* mem, int k, double z_)
      : pid(id), stride(s), points(p), work_mem(mem), K(k), z(z_) {}

  task* execute() {
    double* gl_lower = &work_mem[(NUM_DIVISIONS)*stride];
    double* cost_of_opening_x;
    int local_number_of_centers_to_close = 0;
    long bsize = points->num / ((NUM_DIVISIONS)); //
    long k1 = bsize * pid;
    long k2 = k1 + bsize;

    if (pid == (NUM_DIVISIONS)-1)
      k2 = points->num;

    /*fprintf(stderr,"\t[CenterClose]: pid=%d stride=%d from %d to %d\n",
      pid, stride, k1, k2); */

    number_of_centers_to_close = &work_mem[pid * stride + K];
    cost_of_opening_x = &work_mem[pid * stride + K + 1];

    for (int i = k1; i < k2; i++) {
      if (is_center[i]) {
        double low = z;
        // aggregate from all threads
        for (int p = 0; p < (NUM_DIVISIONS); p++) {
          low += work_mem[center_table[i] + p * stride];
        }
        gl_lower[center_table[i]] = low;
        if (low > 0) {
          // i is a median, and
          // if we were to open x (which we still may not) we'd close i

          // note, we'll ignore the following quantity unless we do open x
          ++local_number_of_centers_to_close;
          *cost_of_opening_x -= low;
        }
      }
    }
    *number_of_centers_to_close = (double)local_number_of_centers_to_close;
    return NULL;
  }
};

class CenterCloseTask : public tbb::task {
  bool is_continuation;
  Points* points;
  double* work_mem;
  int stride;
  double z;
  int K;

public:
  CenterCloseTask(int s, Points* p, double* mem, int k, double z_)
      : stride(s), points(p), work_mem(mem), K(k), z(z_),
        is_continuation(false) {}

  task* execute() {
    tbb::task_list list;
    int p;
    if (!is_continuation) {
      recycle_as_continuation();
      set_ref_count(NUM_DIVISIONS);
      for (p = 1; p < (NUM_DIVISIONS); p++)
        list.push_back(*new (allocate_child())
                           CenterClose(p, stride, points, work_mem, K, z));
      spawn(list);
      CenterClose& me = *new (allocate_child())
                            CenterClose(0, stride, points, work_mem, K, z);
      is_continuation = true;
      return &me;
    } else {
      /* coninuation */

      return NULL;
    }
  }
};

class SaveMoney : public tbb::task {
  Points* points;
  double* work_mem;
  long x;
  int pid, stride;

public:
  SaveMoney(int id, int s, Points* p, long x_, double* mem)
      : pid(id), stride(s), points(p), x(x_), work_mem(mem) {}
  task* execute() {
    double* gl_lower = &work_mem[(NUM_DIVISIONS)*stride];
    long bsize =
        points->num / ((NUM_DIVISIONS)); // points->num/1;//((NUM_DIVISIONS));
    long k1 = bsize * pid;
    long k2 = k1 + bsize;
    int i;

    if (pid == (NUM_DIVISIONS)-1)
      k2 = points->num;

    /*fprintf(stderr,"\t[SaveMoney]: pid=%d stride=%d from %d to %d\n",
      pid, stride, k1, k2);   */

    //  we'd save money by opening x; we'll do it
    for (int i = k1; i < k2; i++) {
      bool close_center = gl_lower[center_table[points->p[i].assign]] > 0;
      if (switch_membership[i] || close_center) {
        // Either i's median (which may be i itself) is closing,
        // or i is closer to x than to its current median
        points->p[i].cost =
            points->p[i].weight * dist(points->p[i], points->p[x], points->dim);
        points->p[i].assign = x;
        // fprintf(stderr,"\t[SaveMoney] %d: cost %lf,
        // x=%d\n",i,points->p[i].cost, x);
      }
    }
    for (int i = k1; i < k2; i++) {
      if (is_center[i] && gl_lower[center_table[i]] > 0) {
        is_center[i] = false;
      }
    }
    if (x >= k1 && x < k2) {
      // fprintf(stderr,"\t-->is_center[%d]=true!\n",x);
      is_center[x] = true;
    }

    return NULL;
  }
};

class SaveMoneyTask : public tbb::task {
  bool is_continuation;
  Points* points;
  long x;
  double* work_mem;
  int stride;

public:
  SaveMoneyTask(int s, Points* p, long x_, double* mem)
      : stride(s), points(p), x(x_), work_mem(mem), is_continuation(false) {}

  task* execute() {
    tbb::task_list list;
    int p;
    if (!is_continuation) {
      recycle_as_continuation();
      set_ref_count(NUM_DIVISIONS);
      for (p = 1; p < (NUM_DIVISIONS); p++)
        list.push_back(*new (allocate_child())
                           SaveMoney(p, stride, points, x, work_mem));
      spawn(list);
      SaveMoney& me =
          *new (allocate_child()) SaveMoney(0, stride, points, x, work_mem);
      is_continuation = true;
      return &me;
    } else {
      /* coninuation */

      return NULL;
    }
  }
};

/* run speedy on the points, return total cost of solution */
float pspeedy(Points* points, float z, long* kcenter) {
  static double totalcost;
  static bool open = false;
  static double* costs; // cost for each thread.
  static int i;

  /* create center at first point, send it to itself */
  {
    int grain_size = points->num / ((NUM_DIVISIONS));
    CenterCreate c(points);
    tbb::parallel_for(tbb::blocked_range<int>(0, points->num, grain_size), c);
  }

  *kcenter = 1;

  {
    int grain_size = points->num / ((NUM_DIVISIONS));
    double acc_cost = 0.0;
    CenterOpen c(points);
    for (i = 1; i < points->num; i++) {
      bool to_open =
          ((float)lrand48() / (float)INT_MAX) < (points->p[i].cost / z);
      if (to_open) {
        (*kcenter)++;
        c.i = i;
        // fprintf(stderr,"** New center for i=%d\n",i);
        tbb::parallel_reduce(
            tbb::blocked_range<int>(0, points->num, grain_size), c);
      }
    }

    c.type = 1; /* Once last time for actual reduction */
    tbb::parallel_reduce(tbb::blocked_range<int>(0, points->num, grain_size),
                         c);

    totalcost = z * (*kcenter);
    totalcost += c.getTotalCost();
  }
  return (totalcost);
}

double pgain(long x, Points* points, double z, long int* numcenters) {
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

  work_mem = (double*)calloc(stride * ((NUM_DIVISIONS) + 1), sizeof(double));

  gl_cost_of_opening_x = 0;
  gl_number_of_centers_to_close = 0;

  /*For each center, we have a *lower* field that indicates
    how much we will save by closing the center.
    Each thread has its own copy of the *lower* fields as an array.
    We first build a table to index the positions of the *lower* fields.
  */

  /*****  loopA() *****/
  {
    CenterTableCountTask& t =
        *new (tbb::task::allocate_root())
            CenterTableCountTask(stride, points, work_mem);
    tbb::task::spawn_root_and_wait(t);
  }

  {
    FixCenterTask& t = *new (tbb::task::allocate_root())
                           FixCenterTask(stride, points, work_mem);
    tbb::task::spawn_root_and_wait(t);
  }

  /***************/

  // now we finish building the table. clear the working memory.
  memset(switch_membership, 0, points->num * sizeof(bool));
  memset(work_mem, 0, (NUM_DIVISIONS + 1) * stride * sizeof(double));

  /* loopB */
  {
    LowerCostTask& t = *new (tbb::task::allocate_root())
                           LowerCostTask(stride, points, x, work_mem, K);
    tbb::task::spawn_root_and_wait(t);
  }

  /* LoopC */
  {
    CenterCloseTask& t = *new (tbb::task::allocate_root())
                             CenterCloseTask(stride, points, work_mem, K, z);
    tbb::task::spawn_root_and_wait(t);
  }

  gl_cost_of_opening_x = z;
  // aggregate
  for (int p = 0; p < (NUM_DIVISIONS); p++) {
    gl_number_of_centers_to_close += (int)work_mem[p * stride + K];
    gl_cost_of_opening_x += work_mem[p * stride + K + 1];
  }

  /*fprintf(stderr,"\tgl_number_of_centers_to_close =
    %d\n",gl_number_of_centers_to_close);
    fprintf(stderr,"\tgl_cost_of_opening_x = %lf\n",gl_cost_of_opening_x); */

  // Now, check whether opening x would save cost; if so, do it, and
  // otherwise do nothing

  if (gl_cost_of_opening_x < 0) {

    /* loopD */
    SaveMoneyTask& t = *new (tbb::task::allocate_root())
                           SaveMoneyTask(stride, points, x, work_mem);
    tbb::task::spawn_root_and_wait(t);

    *numcenters = *numcenters + 1 - gl_number_of_centers_to_close;
  } else {
    gl_cost_of_opening_x = 0; // the value we'll return
  }

  free(work_mem);

  return -gl_cost_of_opening_x;
}

float pFL(Points* points, int* feasible, int numfeasible, double z, long* k,
          double cost, long iter, double e) {

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
    intshuffle(feasible, numfeasible);

    for (i = 0; i < iter; i++) {
      x = i % numfeasible;
      // fprintf(stderr,"Iteration %d z=%lf, change=%lf\n",i,z,change);
      change += pgain(feasible[x], points, z, k);
      // fprintf(stderr,"*** change: %lf, z=%lf\n",change,z);
    }
    cost -= change;
  }

  return (cost);
}

int selectfeasible_fast(Points* points, int** feasible, int kmin) {
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
  accumweight = (float*)memoryFloat.allocate(sizeof(float) * points->num);

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

  memoryFloat.deallocate(accumweight, sizeof(float));

  return numfeasible;
}

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

  //  hizs = (double*)calloc(nproc,sizeof(double));
  hiz = loz = 0.0;
  long numberOfPoints = points->num;
  long ptDimension = points->dim;

  // my block
  long bsize = points->num / nproc;
  long k1 = bsize * pid;
  long k2 = k1 + bsize;
  if (pid == nproc - 1)
    k2 = points->num;

  // fprintf(stderr,"Starting Kmedian procedure\n");
  // fprintf(stderr,"%i points in %i dimensions\n", numberOfPoints,
  // ptDimension);

  int grain_size = points->num / ((NUM_DIVISIONS));
  if (grain_size == 0) {

    for (long kk = 0; kk < points->num; kk++) {
      hiz +=
          dist(points->p[kk], points->p[0], ptDimension) * points->p[kk].weight;
    }

  } else {
    HizReduction h(points);
    tbb::parallel_reduce(tbb::blocked_range<int>(0, points->num, grain_size),
                         h);
    hiz = h.getHiz();
  }

  loz = 0.0;
  z = (hiz + loz) / 2.0;

  /* NEW: Check whether more centers than points! */
  if (points->num <= kmax) {
    /* just return all points as facilities */
    for (long kk = 0; kk < points->num; kk++) {
      points->p[kk].assign = kk;
      points->p[kk].cost = 0;
    }

    cost = 0;
    *kfinal = k;

    return cost;
  }

  shuffle(points);
  cost = pspeedy(points, z, &k);

  i = 0;

  /* give speedy SP chances to get at least kmin/2 facilities */
  while ((k < kmin) && (i < SP)) {
    cost = pspeedy(points, z, &k);
    i++;
  }

  /* if still not enough facilities, assume z is too high */
  while (k < kmin) {
    if (i >= SP) {
      hiz = z;
      z = (hiz + loz) / 2.0;
      i = 0;
    }

    shuffle(points);
    cost = pspeedy(points, z, &k);
    i++;
  }

  /* now we begin the binary search for real */
  /* must designate some points as feasible centers */
  /* this creates more consistancy between FL runs */
  /* helps to guarantee correct # of centers at the end */

  numfeasible = selectfeasible_fast(points, &feasible, kmin);
  for (int i = 0; i < points->num; i++) {
    // fprintf(stderr,"\t-->is_center[%d]=true!\n",points->p[i].assign);
    is_center[points->p[i].assign] = true;
  }

  while (1) {
    /* first get a rough estimate on the FL solution */
    lastcost = cost;
    cost = pFL(points, feasible, numfeasible, z, &k, cost,
               (long)(ITER * kmax * log((double)kmax)), 0.1);

    /* if number of centers seems good, try a more accurate FL */
    if (((k <= (1.1) * kmax) && (k >= (0.9) * kmin)) ||
        ((k <= kmax + 2) && (k >= kmin - 2))) {

      /* may need to run a little longer here before halting without
         improvement */
      cost = pFL(points, feasible, numfeasible, z, &k, cost,
                 (long)(ITER * kmax * log((double)kmax)), 0.001);
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
  }

  //  fprintf(stderr,"Cleaning up...\n");
  // clean up...
  free(feasible);
  *kfinal = k;

  return cost;
}

void* localSearchSub(void* arg_) {

  pkmedian_arg_t* arg = (pkmedian_arg_t*)arg_;
  pkmedian(arg->points, arg->kmin, arg->kmax, arg->kfinal, arg->pid,
           arg->barrier);

  return NULL;
}

void localSearch(Points* points, long kmin, long kmax, long* kfinal) {
  pkmedian_arg_t arg;
  arg.points = points;
  arg.kmin = kmin;
  arg.kmax = kmax;
  arg.pid = 0;
  arg.kfinal = kfinal;
  localSearchSub(&arg);
}
