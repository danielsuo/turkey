#!/bin/sh

NNODES=`uniq $PBS_NODEFILE | wc -l`

echo ------------------------------------------------------
echo 'Job is running on node(s): '
cat $PBS_NODEFILE
echo ------------------------------------------------------
echo PBS: qsub is running on $PBS_O_HOST
echo PBS: originating queue is $PBS_O_QUEUE
echo PBS: executing queue is $PBS_QUEUE
echo PBS: working directory is $PBS_O_WORKDIR
echo PBS: execution mode is $PBS_ENVIRONMENT
echo PBS: job identifier is $PBS_JOBID
echo PBS: job name is $PBS_JOBNAME
echo PBS: node file is $PBS_NODEFILE
echo PBS: number of nodes is $NNODES
echo PBS: current home directory is $PBS_O_HOME
echo PBS: PATH = $PBS_O_PATH
echo PBS: TMPDIR = $TMPDIR
echo ------------------------------------------------------

cp -R $PBS_O_HOME/turkey $TMPDIR
source activate idp

jobname=${prefix}-`printf "%08d" ${PBS_ARRAY_INDEX}`
~/turkey/bin/turkey run ~/turkey/jobs/${apps}/${jobname}.job -i $TMPDIR/turkey -o $PBS_JOBID/${jobname} -n ${workers}
