import os
import time
import pathlib

def generate_cmd(args):

    # Call qsub with full environment the job was run from and merge stderr/out
    cmd = ['qsub -V -j oe']

    # Give the job a name
    cmd.append('-N %(jobname)s')

    # Have exclusive access to the cores on a single machine
    cmd.append('-lselect=1:ncpus=%(ncpus)d -lplace=excl')

    # Arguments to pass into the run script
    cmd.append('-v turkey=%(turkey_home)s,jobfile=%(jobfile)s,out=%(out_dir)s')

    # Redirect stdout
    cmd.append('-o %(out_dir)s/stdout.out')

    # Redirect stderr
    cmd.append('-e %(out_dir)s/stderr.out')

    # Mail whenever job begins, aborts, ends, or is killed
    if args.email is not None:
        cmd.append('-M %(email)s -m abe')

    cmd.append('%(run_script)s')

    cmd = ' '.join(cmd) % {
            'ncpus': args.ncpus,
            'turkey_home': args.turkey_home,
            'jobfile': args.jobfile,
            'jobname': args.jobname,
            'out_dir': args.out_dir,
            'run_script': args.run_script,
            'email': args.email
            }

    return cmd

class Qsub():
    def __init__(self, args):
        self.args = args
        self.args.prefix = time.strftime('%Y-%m-%d-%H-%M-%S', time.localtime())


    def run(self):
        self.paths = [self.args.path]

        if os.path.isdir(self.args.path):
            self.paths = [str(path) for path in pathlib.Path(self.args.path).glob('*.job')]

        for path in self.paths:
            self.args.jobfile = path
            self.args.jobname = os.path.basename(path)
            self.args.out_dir = os.path.join(self.args.turkey_home, 'out', '%s-%s' % (self.args.prefix, self.args.jobname))
            os.system('mkdir -p %s' % self.args.out_dir)

            cmd = generate_cmd(self.args)
            print(cmd)

            os.system(cmd)

