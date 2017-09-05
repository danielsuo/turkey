import os
import json
import time
from .task import Task


class Job:
    def __init__(self, args):
        self.prefix = time.strftime('%Y-%m-%d-%H-%M-%S', time.localtime())
        self.file = args.file
        self.working_dir = args.working_dir

        self.out_dir = args.out_dir if args.out_dir != None else \
            self.prefix + '_' + self.file.split('/')[-1].split('.')[0] + '.out'
        self.out_dir = os.path.join(self.working_dir, self.out_dir)
        self.time = args.time

        os.system('mkdir -p %s' % os.path.join(self.working_dir, self.out_dir))

        with open(self.file, 'r') as f:
            self.tasks = [Task(task, out_dir=self.out_dir)
                          for task in json.load(f)]

    def run(self, stdout=False):
        for task in self.tasks:
            task.run(stdout=stdout)

        os.wait()
        os.system('stty sane')
