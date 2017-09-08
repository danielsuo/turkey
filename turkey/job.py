import os
import csv
import json
import time
import subprocess
import datetime
import pathos.multiprocessing as mp
from multiprocess import Process
from .task import Task
import config


def parse_vmstat(stdout):
    output = map(str.split, stdout.strip().split('\n'))

    # We want the 2nd and 4th lines to grab column headers and statistics
    # from the most recent second, respectively
    return dict(zip(output[1], output[3]))


def get_stats():
    # We have to get the second line of vmstat (after waiting one second).
    # Otherwise, there the statistics are averaged from last reboot.
    output = subprocess.check_output('vmstat 1 2', shell=True)
    result = parse_vmstat(output)
    result['datetime'] = datetime.datetime.now()
    result['tasks'] = config.num_tasks_in_system.value

    return result


def write_stats(stat_file):

    # Grab stats once to get fieldnames
    keys = get_stats().keys()

    with open(stat_file, 'w') as f:
        writer = csv.DictWriter(f, fieldnames=keys, delimiter=',')
        writer.writeheader()
        while True:
            print('Getting stats...Ctrl-\\ to quit')
            writer.writerow(get_stats())

            # TODO: we shouldn't flush, but while debugging...
            f.flush()


class Job:
    def __init__(self, args):
        self.prefix = time.strftime('%Y-%m-%d-%H-%M-%S', time.localtime())
        self.file = args.file

        self.in_dir = args.in_dir if args.in_dir else args.turkey_home
        self.out_dir = args.out_dir if args.out_dir is not None else \
            os.path.join(args.turkey_home, 'out', self.prefix +
                         '_' + self.file.split('/')[-1].split('.')[0] + '.out')

        os.system('mkdir -p %s' % self.out_dir)

        with open(self.file, 'r') as f:
            self.tasks = [Task(task, out_dir=self.out_dir, in_dir=self.in_dir,
                               turkey_home=args.turkey_home)
                          for task in json.load(f)]

        self.pool_size = args.pool_size

        # TODO: might want a better name (i.e., moldable Linux)
        self.intelligent = args.intelligent

    def run(self, stdout=False):
        # Set up up system stat collector
        self.stat_process = Process(
            target=write_stats, args=(os.path.join(self.out_dir, 'stats.csv'),))
        self.stat_process.start()

        pool_size = min(len(self.tasks), self.pool_size, mp.cpu_count())
        pool = mp.Pool(pool_size)

        args = {}

        # Initialize the number of tasks remaining
        config.num_tasks_remaining.set(len(self.tasks))

        for task in self.tasks:
            # TODO: We should discuss the details of intelligent. For example, in
            # the case where tasks are pinned to fewer than cpu_count number of
            # cores
            if self.intelligent:
                args['threads'] = int(
                    mp.cpu_count() / config.num_tasks_in_system)

            # Wait before we deliver the next task
            # TODO: Not great that this happens on the main thread
            task.delay()

            apply_args = {
                'args': args,
                'stdout': stdout,
                'wait': True,
                'count': True
            }
            pool.apply_async(task.run, (), apply_args)

        # Normally we'd use os.wait(), but between wanting to wait for the
        # async applies to finish and os.wait() also depending on the stat
        # process, we have a deadlock
        while config.num_tasks_remaining.value > 0:
            time.sleep(5)

        self.stat_process.terminate()
        os.system('stty sane')
    #  if args.add_all:
        #  pool_size = min(job.ntasks, args.num_workers)
        #  pool = mp.Pool(pool_size)
        #  if args.turkey_mode:
        #  pool.map(lambda task: task.run(threads=int(mp.cpu_count()
        #  / pool_size), wait=True), job.task_array)
        #  else:
        #  pool.map(lambda task: task.run(wait=True), job.task_array)
    #  else:
        #  job.run()
