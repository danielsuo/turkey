import json


class Policy():
    def __init__(self, f, out_dir='out', TURKEY_HOME='.'):
        print('Hello, world!')
        with open(f, 'r') as policy_file:
            self.policy = json.load(policy_file)

        print(self.policy)

    def run(self):
        print('Running')
