import os
import zmq

from scheduler import Scheduler
from fbs import encodeMessage, decodeMessage
from fbs.Turkey.MessageType import MessageType

class Server():
    def __init__(self, num_cpus, pid=None, protocol='ipc', address='turkey-server', port=None):
        self.protocol = protocol
        self.address = address
        self.port = '' if port is None else str(port)
        self.pid = os.getpid() if pid is None else pid
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.ROUTER)

        self.url = '%(protocol)s://%(address)s%(sep)s%(port)s' % {
            'protocol': self.protocol,
            'address': self.address,
            'sep': '' if port is None else ':',
            'port': self.port
        }

        self.tasks = {}
        self.resources = num_cpus

        self.scheduler = Scheduler()

    def bind(self):
        self.socket.bind(self.url)

    def start(self, func, args={}):
        self.bind()
        while True:
            func(self, args)

            # 1. On start / stop message:
            # 2. Update task list
            # 3. Call scheduler
            # 4. Send messages to tasks to update resource allocs



def sched(self, args):
    # Get identity of sender
    identity = self.socket.recv()

    # Receive and toss out empty delimeter frame
    self.socket.recv()

    # Receive buffer data and decode
    buf = self.socket.recv()
    message = decodeMessage(buf)

    if (message.Type() == MessageType.Start):
        self.tasks[identity] = {}
        print('Registered client %d,%s!' % (message.Data(), identity))
    elif (message.Type() == MessageType.Stop):
        self.tasks.pop(identity, None)
        print('Removed client %d,%s!' % (message.Data(), identity))
        self.socket.send_multipart([identity, encodeMessage(MessageType.Stop, 0)])

    # For now, schedule whenever we receive a message
    self.scheduler.schedule(self.tasks, self.resources)

    # Send a message to update each remaining task
    for taskid in self.tasks:
        self.socket.send_multipart([taskid, encodeMessage(MessageType.Update, self.tasks[taskid]['resources'])])


if __name__ == '__main__':
    print('Hello, world!')
    server = Server(64, protocol='tcp', address='*', port=5555)
    print(server.pid)
    print(server.url)
    server.start(sched)
