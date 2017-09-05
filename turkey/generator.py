import json
import random
import numpy as np


class Generator():
    def __init__(self, f):
        self.files = {}
        self.files['params'] = f
        self.files['job'] = f.replace('params', 'job')

        with open(f, 'r') as params:
            self.params = json.load(params)

    def generate(self):
        # TODO: generate multiple jobs
        #  for i in range(self.params['jobs']):

        tasks = []
        arrival = self.params['arrival']
        for j in range(self.params['tasks']):
            app = random.sample(self.params['apps'], 1)[0]

            # Get arrival time of next task
            timer = getattr(np.random, arrival['distribution'])(
                **arrival['parameters']) * arrival['scale']

            # Construct application-specific arguments
            task = {
                'start': timer,
                'app': app['name'],
                'conf': self.params['conf'],
                'mode': self.params['mode'],
            }

            if 'size' in app:
                size = app['size']

                # Get size of next task
                task_size = int(getattr(np.random, size['distribution'])(
                    **size['parameters']) * size['scale'])

                task[app['size']['arg']] = int(task_size)

            for parameter in self.params['scheduler']['parameters']:
                task[parameter] = self.params['scheduler']['parameters'][parameter]

            if 'taskset' in self.params:
                task['taskset'] = self.params['taskset']

            tasks.append(task)

        with open(self.files['job'], 'w') as f:
            f.write(json.dumps(tasks))
