import os
import time
import json
import copy
import subprocess

# Documentation for Task: https://goo.gl/UJNB6J


class Task:
    tid = 0

    def __init__(self, desc, out_dir='out'):

        self.desc = desc

        # Set task id
        self.desc['id'] = Task.tid
        Task.tid += 1

        # Compute directories
        self.dirs = {}
        self.dirs['root'] = os.getenv('TURKEY_HOME', '.')
        self.dirs['src'] = os.path.join(
            self.dirs['root'], 'apps', self.desc['app'])
        self.dirs['input'] = os.path.join(self.dirs['src'], 'inputs')
        self.dirs['output'] = os.path.join(out_dir, str(self.desc['id']))
        self.dirs['conf'] = os.path.join(self.dirs['src'], 'conf')
        self.dirs['exec'] = os.path.join(
            self.dirs['root'], 'build/apps', self.desc['app'])

        # Make output directory if necessary
        os.system('mkdir -p %s' % self.dirs['output'])

        # Compute file locations
        self.files = {}
        self.files['conf'] = os.path.join(
            self.dirs['conf'], '%s.json' % self.desc['conf'])
        self.files['output'] = os.path.join(self.dirs['output'], 'task.out')

        with open(self.files['conf'], 'r') as conf_file:
            self.conf = json.load(conf_file)

        # Determine executable file
        self.files['exec'] = self.dirs['exec']
        if 'exe' in self.conf:
            self.files['exec'] += '/%s' % self.conf['exe']
        else:
            self.files['exec'] += '/%s_%s' % (self.desc['app'],
                                              self.desc['mode'])

        # Make sure number of threads is within bounds of application
        threads = self.desc['threads']
        if 'max_threads' in self.conf:
            threads = min(self.conf['max_threads'], int(threads))

        if 'min_threads' in self.conf:
            threads = max(self.conf['min_threads'], int(threads))

        self.desc['threads'] = str(threads)

        # Set up arguments
        self.args = copy.deepcopy(self.desc)
        self.args['nthreads'] = self.desc['threads']
        self.args['inputs'] = self.dirs['input']
        self.args['outputs'] = self.dirs['output']

        if 'start' not in self.args:
            self.args['start'] = 0

    def run(self, args={}, stdout=False, wait=False):

        # Overwrite "compile"-time attributes with "run"-time attributes in
        # addition to adding any new attributes
        for key in args:
            self.args[key] = args[key]

        # Add environment variables if any
        if 'environment' in self.conf:
            for key in self.conf['environment'].keys():
                os.environ[key] = self.conf['environment'][key] % args

        args = (self.conf['args'] % self.args).split()
        args.insert(0, self.files['exec'])

        # Time the run
        args.insert(0, '-p')
        args.insert(0, 'time')

        if 'taskset' in self.args:
            args.insert(0, self.args['taskset'])
            args.insert(0, '-c')
            args.insert(0, 'taskset')

        # TODO: this isn't great, but life is complicated
        args = ' '.join(args)
        print('Running task %d with args "%s"' % (self.desc['id'], args))
        args = 'date "+datetime: %Y-%m-%dT%H:%M:%S" && ' + args

        # Run executable
        if stdout:
            proc = subprocess.Popen(args, env=os.environ, shell=True)
        else:
            with open(self.files['output'], 'w') as out:
                proc = subprocess.Popen(args, stdin=open(
                    os.devnull), stdout=out, stderr=out, env=os.environ, shell=True)

        if wait:
            os.wait()
            os.system('date')

    def delay(self):
        # Delay start
        time.sleep(self.args['start'])

