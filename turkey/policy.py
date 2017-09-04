import os
import json
import time
import numpy as np
from .task import Task

# TODO: handle multiple apps


class Policy():
    def __init__(self, f, out_dir='out', TURKEY_HOME='.'):

        self.TURKEY_HOME = TURKEY_HOME

        with open(f, 'r') as policy_file:
            self.policy = json.load(policy_file)

        self.prefix = os.path.join(self.TURKEY_HOME, out_dir,
                                   time.strftime('%Y_%m_%d_%H_%M_%S'))

    def run(self):
        print('Running')

        arrival = self.policy['arrival']
        size = self.policy['size']

        for i in range(self.policy['num']):
            # Get arrival time of next task
            timer = getattr(np.random, arrival['distribution'])(
                **arrival['parameters']) * arrival['scale']

            # Get size of next task
            task_size = int(getattr(np.random, size['distribution'])(
                **size['parameters']) * size['scale'])

            # Construct output directory
            out_dir = os.path.join(self.prefix, '%s_%d' %
                                   (self.policy['app'], i))

            # Construct application-specific arguments
            args = {
                self.policy['size']['arg']: int(task_size)
            }

            # Wait to release next task
            time.sleep(timer)
            print('Running job %d after delay of %d with size %d' %
                  (i, timer, task_size))

            task = Task([0, i, self.policy['app'], self.policy['conf'], self.policy['mode'],
                         self.policy['scheduler']['parameters']['threads']], out_dir=out_dir, TURKEY_HOME=self.TURKEY_HOME)

            taskset = self.policy['taskset'] if 'taskset' in self.policy else None
            task.run(args=args, taskset=taskset)

        os.wait()
