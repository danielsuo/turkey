import os
import time
import zmq

from scheduler import Scheduler
from fbs import encodeMessage, decodeMessage
from fbs.Turkey.MessageType import MessageType

class Server():
    def __init__(self, pid=None, protocol='ipc', address='turkey-server', port=None):
        self.protocol = protocol
        self.address = address
        self.port = '' if port is None else str(port)
        self.pid = os.getpid() if pid is None else pid
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)

        self.url = '%(protocol)s://%(address)s%(sep)s%(port)s' % {
            'protocol': self.protocol,
            'address': self.address,
            'sep': '' if port is None else ':',
            'port': self.port
        }

        self.tasks = []
        self.resources = []

        self.scheduler = Scheduler()

    def bind(self):
        self.socket.bind(self.url)

    def start(self, func, args={}):
        self.bind()
        while True:
            func(self, args)

            # TODO: implement scheduler loop
            # 1. On start / stop message:
            # 2. Update task list
            # 3. Call scheduler
            # 4. Send messages to tasks to update resource allocs



def loop(self, args):
    message = self.socket.recv()
    print('Received request: %s' % message)
    time.sleep(1)
    self.socket.send(b'Received your request!')

def sched(self, args):
    buf = self.socket.recv()
    message = decodeMessage(buf)


    self.socket.send(encodeMessage(MessageType.Update, 0))

    print(message.Data())
    time.sleep(1)


if __name__ == '__main__':
    print('Hello, world!')
    server = Server(protocol='tcp', address='*', port=5555)
    print(server.pid)
    print(server.url)
    server.start(sched)
