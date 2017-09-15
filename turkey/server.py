import os
import time
import zmq


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

    def bind(self):
        self.socket.bind(self.url)

    def start(self):
	self.bind()
        while True:
            message = self.socket.recv()
            print('Received request: %s' % message)
            time.sleep(1)
            self.socket.send(b'Received your request!')


if __name__ == '__main__':
    print('Hello, world!')
    server = Server(protocol='tcp', address='*', port=5555)
    print(server.pid)
    print(server.url)
    server.start()
