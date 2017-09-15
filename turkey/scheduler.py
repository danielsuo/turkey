import datetime

class Scheduler():
    def __init__(self):
        self.timeout = 0
        self.last_schedule = datetime.datetime.now()

        print('Hello, world!')

    def schedule(self):
        # 1. Get list of current tasks
        # 2. Get list of current resources
        # 3. Return mapping of tasks to resources
        print('scheduling...')
