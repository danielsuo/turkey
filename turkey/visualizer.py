import os
import pandas as pd
import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt


class Visualizer():
    def __init__(self, args):
        self.file = args.file
        self.files = {
                'hist_delay': self.file.replace('parsed.csv', 'hist_delay.pdf'),
                'hist_size': self.file.replace('parsed.csv', 'hist_size.pdf.'),
                'hist_response': self.file.replace('parsed.csv', 'hist_response.pdf'),
                'num_tasks': self.file.replace('parsed.csv', 'num_tasks.pdf')
                }

        self.df = pd.read_csv(self.file)

    def visualize(self):

        # Histogram of time between tasks
        n, bins, patches = plt.hist(self.df['start'], 40, normed=1)
        plt.xlabel('Delay (s)')
        plt.ylabel('Probability')
        plt.savefig(self.files['hist_delay'])
        plt.close()

        # Histogram of task sizes
        #  n, bins, patches = plt.hist(self.df['noptions'], 40, normed=1)
        #  plt.xlabel('Number of options')
        #  plt.ylabel('Probability')
        #  plt.savefig(self.files['hist_size'])
        #  plt.close()

        # Histogram of response times
        n, bins, patches = plt.hist(self.df['real'] / 1000, 40, normed=1)
        plt.xlabel('Response time (s)')
        plt.ylabel('Probability')
        plt.savefig(self.files['hist_response'])
        plt.close()

        # Number of tasks in the system over time

        print('INFO: Graphs saved to %s' % os.path.dirname(self.file))
