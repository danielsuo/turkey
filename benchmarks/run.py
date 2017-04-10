#!/usr/bin/env python

import sys
import argparse

import docker
from turkey import Job, parse_dir

sys.stdout = sys.stderr

parser = argparse.ArgumentParser(description='PARSEC job runner')
parser.add_argument('app', help='PARSEC app to run')
parser.add_argument('-n', '--num-threads', help='number of threads to start', type=int, default=1)
parser.add_argument('-i', '--input', help='input size to run', default='simdev')
parser.add_argument('-d', '--num-dups', help='number of duplicates to run', type=int, default=1)
parser.add_argument('-m', '--mode', help='cpu limits: set, quota, or shares', default='shares')
parser.add_argument('-c', '--cpus', help='argument for cpu limits', default='1024')
parser.add_argument('-p', '--parse', help='parse output', action='store_true')
args = parser.parse_args()

if args.parse:
	_filter = '.out' if args.app == 'all' else args.app
	parse_dir('./', _filter)
else:
	task_desc = {
	    'app': args.app,
	    'input': args.input,
	    'threads': args.num_threads,
	    'cpus': args.cpus,
	    'mode': args.mode
	}

	job = Job.from_duplicated_task(task_desc, args.num_dups)
	job.run()
