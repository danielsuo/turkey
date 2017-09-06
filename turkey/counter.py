import multiprocess

class Counter(object):
    def __init__(self, val=0):
        self.val = multiprocess.Value('i', val)

    def increment(self, n=1):
        with self.val.get_lock():
            self.val.value += n

    def decrement(self, n=1):
        with self.val.get_lock():
            self.val.value -= n

    def set(self, n):
        with self.val.get_lock():
            self.val.value = n

    @property
    def value(self):
        return self.val.value
