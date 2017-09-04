import os
import json
import subprocess

# Run a single task


class Task:
    def __init__(self, desc, out_dir='out', in_dir='.', executable=None, output_to_stdout=False, TURKEY_HOME='.'):
        self.start = desc[0]
        self.id = desc[1]
        self.app = desc[2]
        self.conf_name = desc[3]
        self.mode = desc[4]
        self.threads = desc[5]

        print(self.threads)

        self.in_dir = in_dir
        self.out_dir = out_dir
        os.system('mkdir -p %s' % self.out_dir)

        self.app_dir = os.path.join(self.in_dir, 'apps', self.app)
        self.exec_dir = os.path.join(TURKEY_HOME, 'build/apps', self.app)
        self.conf_file = os.path.join(
            self.app_dir, 'conf', '%s.json' % self.conf_name)

        with open(self.conf_file, 'r') as conf_file:
            self.conf = json.load(conf_file)

        self.out_file = os.path.join(self.out_dir, 'task.out')
        self.output_to_stdout = output_to_stdout

        if 'exe' in self.conf:
            self.executable = os.path.join(self.exec_dir, self.conf['exe'])
        else:
            self.executable = os.path.join(
                self.exec_dir, '%s_%s' % (executable or self.app, self.mode))

    def run(self, args=None, threads=None, time_run=True, wait=False, copy_data=True):

        if 'max_threads' in self.conf:
            self.threads = min(self.conf['max_threads'], int(self.threads))

        if 'min_threads' in self.conf:
            self.threads = max(self.conf['min_threads'], int(self.threads))

        self.threads = str(self.threads)

        if args is None:
            args = {}

        args['nthreads'] = str(threads or self.threads)
        args['inputs'] = os.path.join(self.app_dir, 'inputs'),
        args['outputs'] = self.out_dir

        #  if 'ignore' in self.conf:
        #  for ignore in self.conf['ignore']:
        #  del args[ignore]

        # if 'inputs' in args and copy_data:
        #     os.system('cp -R %s %s' % (args['inputs'], args['outputs']))
        #     args['inputs'] = os.path.join(args['outputs'], 'inputs')

        if 'environment' in self.conf:
            for key in self.conf['environment'].keys():
                os.environ[key] = self.conf['environment'][key] % args

        args = (self.conf['args'] % args).split()
        args.insert(0, self.executable)

        if time_run:
            args.insert(0, '-p')
            args.insert(0, 'time')

        if self.output_to_stdout:
            subprocess.Popen(args, env=os.environ)
        else:
            with open(self.out_file, 'w') as out:
                subprocess.Popen(args, stdin=open(
                    os.devnull), stdout=out, stderr=out)

        if wait:
            os.wait()
            os.system('date')
            # os.system('rm -rf %s' % args['inputs'])
