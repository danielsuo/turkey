#!/usr/bin/env python

import sys
import argparse

from turkey import parse_dir, split_run

sys.stdout = sys.stderr

parser = argparse.ArgumentParser(description='PARSEC job runner')
parser.add_argument('parser', help='What are we parsing')
parser.add_argument('-w', '--working-dir', help='Working directory', default='./')
parser.add_argument('-f', '--filter', help='Parser-specific filter')
parser.add_argument('vargs', nargs='*', help='Other arguments')
args = parser.parse_args()

if args.parser == 'times':
	if args.filter:
		parse_dir(args.working_dir, args.filter)
	else:
		parse_dir(args.working_dir)

elif args.parser == 'vmstat':
	split_run(args.working_dir, args.vargs[0], args.vargs[1])
