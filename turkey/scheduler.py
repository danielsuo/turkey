import datetime

class Scheduler():
    def __init__(self):
        self.timeout = 0
        self.last_schedule = datetime.datetime.now()

    # TODO: for now, treat resources as an int
    def schedule(self, tasks, resources):
        for taskid in tasks:
            tasks[taskid]['resources'] = int(resources / len(tasks.keys()))

