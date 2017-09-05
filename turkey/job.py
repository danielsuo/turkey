import os
import json
import time
import pathos.multiprocessing as mp
from .task import Task


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

    def run(self, stdout=False):
        pool_size = min(len(self.tasks), self.pool_size, mp.cpu_count())
        pool = mp.ProcessingPool(pool_size)

        for task in self.tasks:
            task.delay()
            pool.apipe(lambda t: t.run(stdout=stdout), task)

        os.wait()
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
