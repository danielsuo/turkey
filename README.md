# Turkey

## TODO
Moved to ```paper```

## Completed
- Confirm results on c4.8xlarge
- Understand mix of applications, rather than just a single application
- Logging
- Lock-free data structures
  - https://github.com/liblfds
- Read about thread pools
  - https://github.com/Pithikos/C-Thread-Pool
  - https://github.com/mbrossard/threadpool
  - https://github.com/facebook/wangle/tree/master/wangle/concurrent
  - https://code.facebook.com/posts/215466732167400/wangle-an-asynchronous-c-networking-and-rpc-library/
  - Microsoft PPL
  - Intel TBB
  - Investigate pthread API
- Parallel programming patterns
  - Textbook
  - Pipeline paper
- Docker Image Pull
- Use init process
  - https://docs.docker.com/engine/reference/run/
- Handle re-entrancy
  - http://www.ibm.com/developerworks/library/l-reent/
- Concurrency books
  - https://www.amazon.com/The-Multiprocessor-Programming-Maurice-Herlihy/dp/0123705916
  - https://www.manning.com/books/c-plus-plus-concurrency-in-action
- eBPF
  - https://github.com/iovisor/bcc/blob/master/tools/offcputime.py
  - http://www.brendangregg.com/FlameGraphs/hotcoldflamegraphs.html
- Parallel programming patterns
  - Textbook
  - Pipeline paper
- Server data
  - Gather speed-up data
  - Fit speed-up curves (phi_cpuset_1_native_#_0-255)
  - Create scheduling algorithm
  - Run blackscholes and ferret at different cpu shares
  - Python + docker to run / update parsec applications
  - Confirm what is being timed
- Test cgroup cpu shares
- Blackscholes
  - https://github.com/danielsuo/parsec/blob/a159cc3884d900e1cf496011bd5b073ba396e09b/pkgs/apps/blackscholes/src/c.m4.pthreads
- Implement server
  - Dummy scheduler (i.e., pin and prioritize)
  - Start processes with parameters
  - Hook up to parsec
- Integrate cgroup operations
  - Spawn processes
  - Clean up after process
  - Modify share
- Change client vector to map?
- Update share and write to shared memory
  - Read/write turkey data
- Fork / exec / wait new processes
  - Share memory
  - NOTE: can work with unmodified programs
  - Make thread safe
- Shared memory locking
- Shared memory with flatbuffers
- Test out passing flatbuffers via zmq
- Replace TCP with zmq
- Implement server
  - Run on threads
  - Keep track of containers
- Implement client
  - TCP client
  - Communicate between containers:
    - https://docs.docker.com/engine/userguide/networking/default_network/container-communication/#communication-between-containers
    - https://docs.docker.com/engine/userguide/networking/
    - https://docs.docker.com/engine/userguide/networking/default_network/dockerlinks/
- Share some memory
  - https://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html
  - http://stackoverflow.com/questions/15035248/linux-shmget-function-in-c
  - http://stackoverflow.com/questions/29173193/shared-memory-with-docker-containers-docker-version-1-4-1
  - https://dzone.com/articles/docker-in-action-the-shared-memory-namespace
- Launch docker images with shared memory
- Send signals between containers
  - https://medium.com/@gchudnov/trapping-signals-in-docker-containers-7a57fdda7d86#.526m535mb
  - https://blog.confirm.ch/sending-signals-docker-container/
  - https://users.cs.cf.ac.uk/Dave.Marshall/C/node24.html
  - http://blog.dixo.net/2015/02/sending-signals-from-one-docker-container-to-another/
- docker++
- Launch docker containers
- Keep track of containers
- Docker++ API
  - Mount volumes in containers (hostconfig:binds)
  - Shared memory
  - Detach
  - Get process
- Pinning and prioritizing processes`
  - http://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/
  - sched_setaffinity
  - http://www.tutorialspoint.com/unix_system_calls/sched_setaffinity.htm`
- Start command in container programmatically
- Finish cleaning up parsec / uploading data and remake docker
  - Development vs production docker (need start-up script http://www.markbetz.net/2014/03/17/docker-run-startup-scripts-then-exit-to-a-shell/)

## Getting started
1. ```./bin/install```
2. ```./bin/compile```
3. ```./build/turkey```
