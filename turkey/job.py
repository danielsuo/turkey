import os
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

        with open(self.file, 'r') as file:
            tasks = [task.strip().split(',') for task in file.readlines()]

        self.tasks = {}
        self.task_array = []
        self.ntasks = len(tasks)

        for tid in range(len(tasks)):
            start = tasks[tid][0]
            if start not in self.tasks:
                self.tasks[start] = []

            task = tasks[tid]
            task.insert(1, str(tid))
            out_dir = os.path.join(
                self.out_dir, '%s_%s_%s_%s_%s_%s' % tuple(task))

            task = Task(tasks[tid], out_dir, args.in_dir,
                        TURKEY_HOME=args.turkey_home)
            self.tasks[start].append(task)
            self.task_array.append(task)

    def run(self):
        tasks_run = 0
        curr_time = 0
        while tasks_run < self.ntasks:
            if str(curr_time) in self.tasks:
                for task in self.tasks[str(curr_time)]:
                    task.run(time_run=self.time)
                    tasks_run += 1
            curr_time += 1
            time.sleep(1)

        os.wait()
        os.system('stty sane')
