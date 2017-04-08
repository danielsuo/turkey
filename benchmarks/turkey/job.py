import os
import subprocess

from utils import *

class Job:
    def __init__(self):
        self.tasks = []

    def run(self, out_dir='./'):
        for task in self.tasks:
            task.run(out_dir)

        os.wait()
        os.system('stty sane')

    @classmethod
    def from_duplicated_task(cls, task, num=1):
        job = cls()
        for i in range(num):
            job.tasks.append(Task(**task))

        return job

class Task:
    def __init__(self, app='blackscholes', input='simdev', threads=1,
                 cpus=1024, mode='shares'):

        self.app     = app
        self.input   = input
        self.threads = threads
        self.cpus    = cpus
        self.mode    = mode

        self.real    = 0
        self.user    = 0
        self.sys     = 0
        self.total   = 0

    def __str__(self):
        members = self.members()
        return ','.join([str(getattr(self, attr)) for attr in members])

    def run(self, docker=True, out_dir='./'):
        path = self.generate_path(out_dir)
        args = self.generate_args(docker)

        # TODO: add logger that's configurable from CLI
        print(args)

        with open(path, 'w') as out:
            subprocess.Popen(args, stdout=out, stderr=out)

    def parse(self, out_dir='./'):
        params = parse_file(self.generate_file(out_dir), out_dir=out_dir)

        self.real = params['real']
        self.user = params['user']
        self.sys  = params['sys']

        self.total = self.user + self.sys

    def members(self):
        return [attr for attr in dir(self) if not callable(getattr(self, attr)) and not attr.startswith("__")]

    def generate_file(self, out_dir='./'):
        filename =  '%s_%s_%s_%s_%s.out' % (self.prefix,
                                            self.app,
                                            self.input,
                                            str(self.threads),
                                            str(self.cpus))

    def generate_path(self, out_dir='./'):
        return os.path.join(out_dir, self.generate_file(out_dir))

    def generate_args(self, docker=True):
        args = []
        if docker:
            args.extend(['sudo', 'docker', 'run', '--rm', '-i', '-t'])

            if self.mode != 'NA':
                if self.mode == 'set':
                    args.append('--cpuset-cpus=%s' % self.cpus)
                elif self.mode == 'shares':
                    args.append('--cpu-shares=%d' % self.cpus)
                elif self.mode == 'quota':
                    args.append('--cpu-quota=%d --cpu-period=%d' %
                        (self.cpus[0], self.cpus[1]))

            args.append('danielsuo/parsec:prod')

        else:
            # TODO: manage shares/quota with nice and cpulimit
            if self.cpus == 'set':
                args.extend(['taskset', '-c', str(self.cpus)])

            args.append(os.path.join(os.environ['PARSEC_HOME'], 'bin/parsecmgmt'))

        args.extend([
            '-a', 'run',
            '-i', self.input,
            '-p', self.app,
            '-n', str(self.threads)
        ])

        return args

task = {
    'app': 'blackscholes',
    'input': 'native',
    'threads': 1,
    'cpus': 1024,
    'mode': 'shares'
}

a = Job.from_duplicated_task(task, 2)
a.tasks[1].real=4
print(a.tasks[0])
print(a.tasks[1])
#
# a = Task()
# print(a.members())
# print(a)
