import os
import csv
import json
import time
import subprocess
import datetime
import pathos.multiprocessing as mp
from multiprocess import Lock, Process, Value
from .task import Task


def parse_vmstat(stdout):
    return dict(zip(*map(str.split, stdout.strip().split('\n'))[-2:]))


def get_stats():
    output = subprocess.check_output('vmstat', shell=True)
    result = parse_vmstat(output)
    result['datetime'] = datetime.datetime.now()

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
            time.sleep(1)


class Job:
    def __init__(self, args):
        self.prefix = time.strftime('%Y-%m-%d-%H-%M-%S', time.localtime())
        self.file = args.file
        self.working_dir = args.working_dir

        self.out_dir = args.out_dir if args.out_dir != None else \
            self.prefix + '_' + self.file.split('/')[-1].split('.')[0] + '.out'
        self.out_dir = os.path.join(self.working_dir, self.out_dir)

        os.system('mkdir -p %s' % os.path.join(self.working_dir, self.out_dir))

        with open(self.file, 'r') as f:
            self.tasks = [Task(task, out_dir=self.out_dir)
                          for task in json.load(f)]

        self.pool_size = args.pool_size

    def run(self, stdout=False, intelligent=False):
        # Set up system lock
        self.lock = Lock()
        self.counter = Value('i', 0)

        # Set up up system stat collector
        self.stat_process = Process(
            target=write_stats, args=(os.path.join(self.out_dir, 'stats.csv'),))
        self.stat_process.start()

        pool_size = min(len(self.tasks), self.pool_size, mp.cpu_count())
        pool = mp.ProcessingPool(pool_size)

        args = {}

        for task in self.tasks:
            # TODO: We may want to allow more threads
            if intelligent:
                args['threads'] = mp.cpu_count()

            print('%d' % pool_size)
            task.delay()
            pool.apipe(lambda t: t.run(args=args, stdout=stdout,
                                       wait=True, lock=self.lock, counter=self.counter), task)

        os.wait()
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
