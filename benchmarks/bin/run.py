#!/usr/bin/env python

import sys
import argparse

from turkey import Job

sys.stdout = sys.stderr

parser = argparse.ArgumentParser(description='PARSEC job runner')
parser.add_argument('app', help='PARSEC app to run')
parser.add_argument('-n', '--num-threads', help='number of threads to start', type=int, default=1)
parser.add_argument('-i', '--input', help='input size to run', default='simdev')
parser.add_argument('-d', '--num-dups', help='number of duplicates to run', type=int, default=1)
parser.add_argument('-m', '--mode', help='cpu limits: set, quota, or shares', default='shares')
parser.add_argument('-u', '--cpus', help='argument for cpu limits', default='1024')
parser.add_argument('-o', '--output-dir', help='where to dump output files', default='./')
parser.add_argument('-t', '--docker-tag', help='which docker tag to run', default='prod')
parser.add_argument('-c', '--config', help='PARSEC config', default='')
args = parser.parse_args()

task_desc = {
	'app': args.app,
	'input': args.input,
	'threads': args.num_threads,
	'cpus': args.cpus,
	'mode': args.mode,
	'docker': True,
	'docker_tag': args.docker_tag,
	'config': args.config
	}

job = Job.from_duplicated_task(task_desc, args.num_dups)
job.run(args.output_dir)
