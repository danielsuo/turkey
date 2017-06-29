#line 233 "/home/ubuntu/parsec/pkgs/libs/parmacs/inst/amd64-linux.gcc/m4/parmacs.pthreads.c.m4"

#line 1 "load.H"
#ifndef _LOAD_H_
#define _LOAD_H_

void maketree(long ProcessId);
cellptr InitCell(cellptr parent, long ProcessId);
leafptr InitLeaf(cellptr parent, long ProcessId);
void printtree(nodeptr n);
nodeptr loadtree(bodyptr p, cellptr root, long ProcessId);
bool intcoord(long xp[NDIM], vector rp);
long subindex(long x[NDIM], long l);
void hackcofm(long ProcessId);
cellptr SubdivideLeaf(leafptr le, cellptr parent, long l, long ProcessId);
cellptr makecell(long ProcessId);
leafptr makeleaf(long ProcessId);


#endif
