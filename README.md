# Turkey

## TODO

- Shared memory with flatbuffers
- Implement server
  - Keep track of containers
  - Dummy scheduler (i.e., pin and prioritize)
  - Start containers with parameters
  - Hook up to parsec
- Docker update command
- Read about thread pools
  - https://github.com/facebook/wangle/tree/master/wangle/concurrent
  - Microsoft PPL
  - Intel TBB
- Investigate pthread API
- Docker Image Pull
- Use init process
  - https://docs.docker.com/engine/reference/run/
- Handle re-entrancy
  - http://www.ibm.com/developerworks/library/l-reent/

## Completed
- Test out passing flatbuffers via zmq
- Replace TCP with zmq
- Implement server
  - Run on threads
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


## Note on security
We could run both server and client applications each in their own containers. But for sake of latency (REST API, really?) and convenience, we run the server as a host application and punch a gigantic hole through the wall of security with the great hammer of priveleged mode. We don't abuse this privelege in the client library, but there's no guarantee that client applications won't abuse them. C'est la vie, amirite?

## Getting started
1. ```./bin/install```
2. ```./bin/build```
3. ```mkdir build && cd build && cmake .. && make```
4. ```./build/turkey```
