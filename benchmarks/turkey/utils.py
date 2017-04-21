import re
import os

# blackscholes
# bodytrack
# canneal
# dedup
# facesim
# ferret
# fluidanimate
# freqmine
# raytrace
# streamcluster
# swaptions
# vips
# x264

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

inputs = [
    'test',
    'simdev',
    'simsmall',
    'simmedium',
    'simlarge',
    'native'
]

pthread = [app for app in apps if app not in ['ferret', 'freqmine', 'raytrace', 'vips', 'x264']]
tbb = ['blackscholes', 'bodytrack', 'fluidanimate', 'streamcluster', 'swaptions']

threads = [1, 2, 4, 8, 16, 32, 64, 128, 256]
cpus_32 = ['0', '0-1', '0-3', '0-7', '0-15', '0-31']
cpus_256 = ['0', '0-1', '0-3', '0-7', '0-15', '0-31', '0-63', '0-127', '0-255']

cpus = {
    'c4.8xlarge': [-eval(x) + 1 for x in cpus_32],
    'fat': [-eval(x) + 1 for x in cpus_32],
    'phi': [-eval(x) + 1 for x in cpus_256]
}

numa = {
    'fat': [
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49],
        [10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59],
        [20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69],
        [30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79]
    ]
}

values = {
    'apps': apps,
    'inputs': inputs,
    'threads': threads,
    'cpus': cpus
}

def parse_ms(line):
    m = re.search('(\d+)m(\d+\.\d+)s', line)
    return float(m.group(1)) * 60 * 1000 + float(m.group(2)) * 1000

def parse_dir(out_dir='./', _filter='.out', _delim='_'):
    outs = [parse_file(f, _delim, out_dir) for f in os.listdir(out_dir) if f.find(_filter) > -1]
    for out in outs:
        print(','.join(out))

def parse_file(out_file, delim='_', out_dir='./'):
    params = os.path.splitext(out_file)[0].split(delim)
    out_file = os.path.join(out_dir, out_file)

    with open(out_file) as f:
        for line in f:
            if line.find('real') > -1 or line.find('user') > -1 or line.find('sys') > -1:
                try:
                    params.append(str(parse_ms(line)))
                except AttributeError:
                    continue

    return params

#
# def generate_out(params):
#     return '%s_%s_%s_%s_%s.out' % (params['prefix'],
#                                    params['app'],
#                                    params['input'],
#                                    str(params['threads']),
#                                    str(params['cpus']))
#
# def generate_cmd(params):
#     print(params)
#     args = []
#     if params['docker']:
#         args.extend(['sudo', 'docker', 'run', '--rm', '-i', '-t'])
#
#         if params['mode'] != 'NA':
#             if params['mode'] == 'set':
#                 args.append('--cpuset-cpus=%s' % params['cpus'])
#             elif params['mode'] == 'shares':
#                 args.append('--cpu-shares=%d' % params['cpus'])
#             elif params['mode'] == 'quota':
#                 args.append('--cpu-quota=%d --cpu-period=%d' %
#                     (params['cpus'][0], params['cpus'][1]))
#
#         args.append('danielsuo/parsec:prod')
#
#     else:
#         # TODO: manage shares/quota with nice and cpulimit
#         if params['cpus'] == 'set':
#             args.extend(['taskset', '-c', str(params['cpus'])])
#
#         args.append(os.path.join(os.environ['PARSEC_HOME'], 'bin/parsecmgmt'))
#
#     args.extend([
#         '-a', 'run',
#         '-i', params['input'],
#         '-p', params['app'],
#         '-n', str(params['threads'])
#     ])
#
#     return args
#
# def rand_cpus(k, n):
#     if type(n) is int:
#         cpus = [str(i) for i in range(n)]
#     else:
#         cpus = [str(i) for i in n]
#
#     random.shuffle(cpus)
#     return ','.join(cpus[:k])
#
# def run(params):
#     cmd = generate_cmd(params)
#     out = generate_out(params)
#
#     print(cmd)
#     print(' '.join(cmd))
#     print(out)
#
#     with open(out, 'w') as out:
#         subprocess.Popen(cmd, stdout=out, stderr=out)
#
# def run_jobs(jobs):
#     for job in jobs:
#         run(job)
#
#     os.wait()
#     os.system('stty sane')
#
#     for job in jobs:
#         out = generate_out(job)
#         print(parse(out))
#
# def run_json(file):
#     prefix = os.path.splitext(os.path.basename(file))[0]
#     print(prefix)
#     with open(file, 'r') as f:
#         jobs = json.loads(f.read())
#         for i in range(len(jobs)):
#             for j in range(len(jobs[i])):
#                 print(jobs[i][j])
#                 jobs[i][j]['prefix'] = '%s-%d-%d' % (prefix, i, j)
#             run_jobs(jobs[i])
#
# def run_app(app, nthreads, ninstances):
# 	job = {
# 		'app': app,
# 		'input': 'native',
# 		'threads': nthreads,
# 		'cpus': 1024,
# 		'mode': 'shares',
# 		'docker': True
# 	}
#
# 	jobs = [copy.deepcopy(job) for i in range(ninstances)]
#
# 	for i in range(ninstances):
# 		jobs[i]['prefix'] = 'test%02d' % i
# 		if jobs[i]['mode'] == 'set':
# 			jobs[i]['cpus'] = '%d' % (i / 2)
#
# 	run_jobs(jobs)
# # Run trials times on k cores out of n total cores
# # If n is array of cores, treat as NUMA node
# def run_job_rand_cpus(job, trials, k, n):
#     job.append('')
#     for i in range(trials):
#         job[len(job) - 1] = rand_cpus(k, n)
#         run_jobs(job)
#         time.sleep(5)
#
# def run_job_seq_cpus(job, trials, k, n):
#     job.append('')
#
#     for i in range(min(trials, int(n / k))):
#         job[len(job) - 1] = '%d-%d' % (i * k, (i + 1) * k - 1)
#         run_jobs(job)
#         time.sleep(5)
