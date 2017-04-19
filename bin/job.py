import os
import time
import json
import subprocess

# Run a single task
class Task:
    def __init__(self, desc, out_dir, in_dir, time_run=True, executable=None, output_to_stdout=False, TURKEY_HOME='.'):
        self.start     = desc[0]
        self.id        = desc[1]
        self.app       = desc[2]
        self.conf_name = desc[3]
        self.mode      = desc[4]
        self.threads   = desc[5]

        self.in_dir    = in_dir
        self.out_dir   = out_dir
        os.system('mkdir -p %s' % self.out_dir)

        self.app_dir = os.path.join(self.in_dir, 'apps', self.app)
        self.exec_dir = os.path.join(TURKEY_HOME, 'build/apps', self.app)
        self.conf_file = os.path.join(self.app_dir, 'conf', '%s.json' % self.conf_name)

        with open(self.conf_file, 'r') as conf_file:
            self.conf = json.load(conf_file)

        self.out_file = os.path.join(self.out_dir, 'task.out')
        self.output_to_stdout = output_to_stdout
        self.executable = os.path.join(self.exec_dir, '%s_%s' % (executable or self.app, self.mode))

        args = {
            'nthreads': self.threads,
            'inputs': os.path.join(self.app_dir, 'inputs'),
            'outputs': self.out_dir
        }

        self.args = (self.conf['args'] % args).split(' ')
        self.args.insert(0, self.executable)

        if time_run:
            self.args.insert(0, '-p')
            self.args.insert(0, 'time')

    def run(self):
        print('Running %s, output to %s' % (self.executable, self.out_file))

        if self.output_to_stdout:
            subprocess.Popen(self.args)
        else:
            with open(self.out_file, 'w') as out:
                subprocess.Popen(self.args, stdin=open(os.devnull), stdout=out, stderr=out)

# Run a timeline of tasks
class Job:
    def __init__(self, args):
        self.prefix      = time.strftime('%Y-%m-%d-%H-%M-%S', time.localtime())
        self.file        = args.file
        self.working_dir = args.working_dir

        self.out_dir     = args.out_dir if args.out_dir != None else \
            self.prefix + '_' + self.file.split('/')[-1].split('.')[0] + '.out'
        self.out_dir     = os.path.join(self.working_dir, self.out_dir)

        os.system('mkdir -p %s' % os.path.join(self.working_dir, self.out_dir))

        with open(self.file, 'r') as file:
            tasks = [task.strip().split(',') for task in file.readlines()]

        self.ntasks = len(tasks)
        self.tasks = {}
        for tid in range(len(tasks)):
            start = tasks[tid][0]
            if start not in self.tasks:
                self.tasks[start] = []

            task = tasks[tid]
            task.insert(1, str(tid))
            out_dir = os.path.join(self.out_dir, '%s_%s_%s_%s_%s_%s' % list(task))
            
            self.tasks[start].append(Task(tasks[tid], out_dir, args.in_dir, time_run=args.time, TURKEY_HOME=args.turkey_home))

    def run(self):
        tasks_run = 0
        curr_time = 0
        while tasks_run < self.ntasks:
            if str(curr_time) in self.tasks:
                for task in self.tasks[str(curr_time)]:
                    task.run()
                    tasks_run += 1
            curr_time += 1
            time.sleep(1)

        os.wait()
        os.system('stty sane')

apps = [
    'blackscholes',
    'bodytrack',
    'canneal',
    'dedup',
    'facesim',
    'ferret',
    'fluidanimate',
    'freqmine',
    'raytrace',
    'streamcluster',
    'swaptions',
    'vips',
    'x264']
