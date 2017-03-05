# Turkey

## TODO
- Start command in container programmatically

- Read about thread pools
  - https://github.com/facebook/wangle/tree/master/wangle/concurrent
- Use init process
  - https://docs.docker.com/engine/reference/run/
- Handle re-entrancy
  - http://www.ibm.com/developerworks/library/l-reent/
- Eventually take over pinning / prioritizing from docker (how does it mess?)

## Completed
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
- Pinning and prioritizing processes
  - http://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/
  - sched_setaffinity
  - http://www.tutorialspoint.com/unix_system_calls/sched_setaffinity.htm
