// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
//
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition,
// Prentice
// Hall, John C. Hull,

#include "Pool.h"
#include "options.h"
#include <folly/futures/Future.h>
#include <glog/logging.h>

#define PAD 256
#define LINESIZE 64

OptionData option;
int numOptions;
int numError = 0;
int nThreads;
int kNumWorkChunks = 1024;

int bs_thread(int tid);

int main(int argc, char** argv) {
  int loopnum;
  int rv;
  int i;

  if (argc != 3) {
    printf("Usage:\n\t%s <nthreads> <noptions>\n", argv[0]);
    exit(1);
  }
  nThreads   = atoi(argv[1]);
  numOptions = atoi(argv[2]);

  if (nThreads > numOptions) {
    printf("WARNING: Not enough work, reducing number of threads to match "
           "number of options.\n");
    nThreads = numOptions;
  }

  kNumWorkChunks = std::min(numOptions, kNumWorkChunks);

  // First option from simsmall
  option.s          = 42;
  option.strike     = 40;
  option.r          = 0.1000;
  option.divq       = 0.00;
  option.v          = 0.20;
  option.t          = 0.5;
  option.OptionType = 'C';
  option.divs       = 0.00;
  option.DGrefval   = 4.759423036851750055;

  pthread_mutexattr_init(&normalMutexAttr);
  numThreads = nThreads;
  {
    int i;
    for (i = 0; i < MAX_THREADS; i++) {
      threadsTableAllocated[i] = 0;
    }
  };
  printf("Num of Options: %d\n", numOptions);
  printf("Num of Runs: %d\n", NUM_RUNS);

  Turkey::DynamicThreadPool dtp(nThreads);
  auto& pool = dtp.getPool();
  // Start the dynamic scheduler
  dtp.start();
  auto chunks = std::vector<int>(kNumWorkChunks);
  std::vector<folly::Future<folly::Unit>> futs;
  for (i = 0; i < kNumWorkChunks; i++) {
    chunks[i] = i;
    futs.push_back(folly::via(&pool).then(
        [i]() { bs_thread(i); }));
  }
  folly::collectAll(futs).wait();

  dtp.stop();

  return 0;
}

int bs_thread(int tid) {
  int i, j;
  fptype price;
  fptype priceDelta;
  int start = tid * (numOptions / kNumWorkChunks);
  int end   = std::min(numOptions, start + (numOptions / kNumWorkChunks));

  if (start >= end) {
    return 0;
  }

  for (j = 0; j < NUM_RUNS; j++) {
    for (i = start; i < end; i++) {
      price = BlkSchlsEqEuroNoDiv(option.s, option.strike, option.r, option.v,
                                  option.t, 0, 0);
    }
  }

  return 0;
}
