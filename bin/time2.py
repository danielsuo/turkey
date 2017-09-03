import os
import subprocess


apps = [
    'blackscholes',
    'ferret',
    'freqmine'
    'bodytrack'
    'canneal',
    'dedup',
    'facesim',
    'fluidanimate',
    'streamcluster',
    'swaptions',
    'vips',
    'x264'
]

times = 1
threads = [1, 2, 4, 8, 12, 16, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64]


def get_args(app, thread, i):
    args = 'taskset -c 0-63 ./bin/turkey one %(app)s -n %(thread)d -c native -o %(outputs)s' % {
        'app': app,
        'thread': thread,
        'outputs': os.path.join('%s_%d_%d' % (app, thread, i))
    }

    return args.split()


for app in apps:
    for thread in threads:
        args = get_args(app, thread, 1)
        subprocess.Popen(args)
