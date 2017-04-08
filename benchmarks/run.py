#!/usr/bin/env python

import sys
import argparse

import docker
from turkey import run_app, parse_dir

sys.stdout = sys.stderr

parser = argparse.ArgumentParser(description='PARSEC job runner')
parser.add_argument('app', help='PARSEC app to run')
parser.add_argument('-t', '--num-threads', help='number of threads to start', type=int, default=1)
parser.add_argument('-i', '--num-iters', help='number of iterations to run', type=int, default=1)
parser.add_argument('-p', '--parse', help='parse output', action='store_true')
args = parser.parse_args()

if args.parse:
	_filter = '.out' if args.app == 'all' else args.app
	parse_dir('./', _filter)
else:
	run_app(args.app, args.num_threads, args.num_iters)
