import sys
import time
import docker
import copy
from turkey import getMilliseconds, run_jobs, apps, parse_dir

sys.stdout = sys.stderr

image = 'danielsuo/parsec:prod'
input = 'native'
threads = 32

j1 = {
	'image': image,
	'prefix': '2017-04-02',
	'app': 'blackscholes',
	'input': input,
	'threads': threads
}

j2 = {
	'image': image,
	'prefix': '2017-04-02',
	'app': 'canneal',
	'input': input,
	'threads': threads
}

client = docker.from_env()

def run(params):
	args = ['-a', 'run', '-p', params['app'], '-i', params['input'], '-n', str(params['threads'])]
	job = client.containers.run(params['image'], args, detach=True, cpu_shares=params['shares'])

	return job

def parse(logs, params):
	for line in logs.split('\n'):
		key = None
		if line.find('real') > -1:
			key = 'real'
		elif line.find('user') > -1:
			key = 'user'
		elif line.find('sys') > -1:
			key = 'sys'
		else:
			continue
		try:
			value = (str(getMilliseconds(line)))
			params[key] = value
		except AttributeError:
			continue
	return params

def printd(d1, d2):
	keys = ['threads', 'real', 'user', 'sys']
	d1 = [str(d1[key]) for key in keys]
	d2 = [str(d2[key]) for key in keys]

	d1.extend(d2)

	print('%s,%s,%s,%s,%s,%s,%s,%s' % tuple(d1))

def run_many_sdk(j, nthreads, ninstances):
	containers = []
	for i in range(ninstances):
		j['shares'] = 1024
		j['threads'] = nthreads

		containers.append(run(j))

	for i in range(ninstances):
		containers[i].wait()

	for i in range(ninstances):
		parse(containers[i].logs(), j)
		keys = ['threads', 'real', 'user', 'sys']
		d = [str(j[key]) for key in keys]
		print('%s,%s,%s,%s' % tuple(d))

	print

# run_many(j2, 1, 16)
# time.sleep(5)
# run_many(j2, 32, 16)

def run_many_all_apps(nthreads, ninstances):
	for app in apps:
		job = {
			"app": app,
			"input": "native",
			"threads": nthreads,
			"cpus": 1024,
			"mode": "share",
			"docker": True
		}

		jobs = [copy.deepcopy(job) for i in range(ninstances)]

		for i in range(ninstances):
			jobs[i]['prefix'] = 'test%02d' % i

		print(jobs)

		run_jobs(jobs)
		time.sleep(5)

# parse_dir()

run_many_all_apps(1, 16)
run_many_all_apps(32, 16)

# for i in range(6):
# 	for j in range(6):
# 		# if i == 0:
# 		j1['shares'] = 1024
# 		j2['shares'] = 1024
#
# 		j1['threads'] = 2 ** i
# 		j2['threads'] = 2 ** j
# 		# 	c1 = run(j1)
# 		# 	c1.wait()
# 		# 	time.sleep(5)
# 		# 	c2 = run(j2)
# 		# 	c2.wait()
# 		# else:
# 		# j1['shares'] = 1024 * i / 8
# 		# j2['shares'] = 2048 - j1['shares']
#
# 		c1 = run(j1)
# 		c2 = run(j2)
# 		c1.wait()
# 		c2.wait()
#
# 		parse(c1.logs(), j1)
# 		parse(c2.logs(), j2)
# 		printd(j1, j2)
#
# 		c1.remove(force=True)
# 		c2.remove(force=True)
#
# 		time.sleep(5)
