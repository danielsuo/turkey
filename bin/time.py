import os

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

for app in apps:
  for thread in threads:
    for i in range(times):
      os.system('taskset -c 0-63 ./bin/turkey one %(app)s -n %(thread)d -c native -o %(outputs)s' % {
        'app': app,
        'thread': thread,
        'outputs': os.path.join('%s_%d_%d' % (app, thread, i))
        })
