#line 233 "/home/ubuntu/parsec/pkgs/libs/parmacs/inst/amd64-linux.gcc/m4/parmacs.pthreads.c.m4"

#line 1 "grav.H"
#ifndef _GRAV_H_
#define _GRAV_H_

void hackgrav(bodyptr p, long ProcessId);
void gravsub(register nodeptr p, long ProcessId);
void hackwalk(long ProcessId);
void walksub(nodeptr n, real dsq, long ProcessId);
bool subdivp(register nodeptr p, real dsq, long ProcessId);

#endif
