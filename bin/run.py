#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import argparse
import subprocess
import pathos.multiprocessing as mp

from turkey import Job, Task, Generator, Parser, Visualizer, Qsub, apps

# TODO: https://argcomplete.readthedocs.io/en/latest/

parser = argparse.ArgumentParser(description='Turkey job runner')
parser.add_argument('-T', '--turkey-home', help='Turkey home directory')
subparsers = parser.add_subparsers(help='sub-command help', dest='cmd')

###############################################################################
# Server commands
###############################################################################

server = subparsers.add_parser('server', help='Handle server stuff')

###############################################################################
# Client (the dummy one) commands
###############################################################################

client = subparsers.add_parser('client', help='Do dummy client stuff')

###############################################################################
# Build subcommand
###############################################################################

build = subparsers.add_parser('build', help='Build specified app')
build.add_argument('app', help='App to build', default='all')
build.add_argument('-f', '--force', help='Force rebuild', action='store_true')
build.add_argument(
    '-e', '--cmake-executable', help='Path to cmake', default='cmake')
build.add_argument('-j', '--parallel', help='Parallelize', action='store_true')

###############################################################################
# Data subcommand
###############################################################################

data = subparsers.add_parser('data', help='Download and unpack data')
data.add_argument('app', help='App to data for', default='all')

###############################################################################
# Generate subcommand
###############################################################################

gen = subparsers.add_parser('gen', help='Generate job files for run command')
gen.add_argument('file', help='Generator parameter file')

###############################################################################
# Job subcommand
###############################################################################

job = subparsers.add_parser('job', help='Run job')
job.add_argument('file', help='Job file')
job.add_argument('-o', '--out-dir', help='Output directory')
job.add_argument('-i', '--in-dir', help='Input directory')
job.add_argument('-p', '--pool-size',
                 help='Number of workers to gate', type=int, default=mp.cpu_count())
job.add_argument('-c', '--num-cpus',
                 help='Number of CPUs we are using', default=mp.cpu_count())
job.add_argument('--intelligent',
                 help='Use intelligent Linux scheduler', action='store_true')

###############################################################################
# One-off run subcommand
###############################################################################

one = subparsers.add_parser('one', help='Run one app')
one.add_argument('app', help='App to run')
one.add_argument(
    '-n', '--num-threads', help='Number of threads', type=int, default=1)
one.add_argument('-c', '--conf', help='Configuration to run', default='test')
one.add_argument('-o', '--out-dir', help='Output directory')
one.add_argument('-i', '--in-dir', help='Input directory')
one.add_argument(
    '-s',
    '--output_to_stdout',
    help='Dump to stdout instead of file',
    action='store_true')
one.add_argument(
    '-m', '--mode', help='Which thread library to use', default='pthread')

###############################################################################
# qsub cluster commands
###############################################################################

qsub = subparsers.add_parser('qsub', help='Run qsub jobs')
qsub.add_argument(
    'path', help='Job file or directory with job files to run relative to args.turkey_home')
qsub.add_argument(
    '-c',
    '--ncpus',
    help='Maximum number of simultaneous jobs',
    type=int,
    default=272)
qsub.add_argument('-r', '--run-script', help='Run script relative to args.turkey_home',
                  default='cluster/run.sh')
qsub.add_argument(
    '-m', '--email', help='Specify if you want to receive lots of emails...')
qsub.add_argument('-p', '--pool-size', type=int, default=mp.cpu_count())
qsub.add_argument('-q', '--queue', default='princeton')

###############################################################################
# Parse subcommand
###############################################################################

parse = subparsers.add_parser('parse', help='Parse results')
parse.add_argument('out_dir', help='Output directory')

###############################################################################
# Viz subcommand
###############################################################################

viz = subparsers.add_parser('viz', help='Visualize results')
viz.add_argument('-o', '--out_dir', help='Output directory')

###############################################################################
# Clean subcommand
###############################################################################

clean = subparsers.add_parser('clean', help='Clean up directory')

args = parser.parse_args()

if args.turkey_home is None:
    try:
        args.turkey_home = os.environ['TURKEY_HOME']
    except KeyError, e:
        print 'ERROR: could not find args.turkey_home in environment. Please specify with -q.'
        sys.exit()

if args.cmd == 'server':
    subprocess.Popen([os.path.join(args.turkey_home, 'build/turkey_server')])
    os.wait()
elif args.cmd == 'client':

    subprocess.Popen([os.path.join(args.turkey_home, 'build/turkey_client')])
    os.wait()
elif args.cmd == 'qsub':
    qsub = Qsub(args)
    qsub.run()
elif args.cmd == 'build':

    if args.force:
        os.system('rm -rf %s' % os.path.join(args.turkey_home, 'build'))
    cwd = os.getcwd()
    os.chdir(args.turkey_home)
    os.system('mkdir -p build')
    os.chdir(os.path.join(args.turkey_home, 'build'))
    os.system('%s .. -DMAKE=%s -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && make %s' % (args.cmake_executable, args.app,
                                              ('-j' if args.parallel else '')))
    os.chdir(cwd)
elif args.cmd == 'data':

    data_executable = os.path.join(args.turkey_home, 'bin/data')
    if args.app in apps:
        os.system('%s %s' % (data_executable, args.app))
    else:
        for app in apps:
            os.system('%s %s' % (data_executable, app))
elif args.cmd == 'gen':
    generator = Generator(args.file)
    generator.generate()
elif args.cmd == 'job':
    job = Job(args)
    job.run()
elif args.cmd == 'one':
    task_args = {
        'app': args.app,
        'conf': args.conf,
        'mode': args.mode,
        'threads': args.num_threads
    }

    task = Task(task_args, out_dir=args.out_dir,
                in_dir=args.in_dir, turkey_home=args.turkey_home)
    task.run(stdout=args.output_to_stdout, wait=True)
elif args.cmd == 'parse':
    parser = Parser(args)
    parser.parse()
elif args.cmd == 'viz':
    visualizer = Visualizer(args)
    visualizer.visualize()
elif args.cmd == 'clean':
    os.system('rm -rf build')
else:
    print parser.print_help()
