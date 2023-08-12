#!/usr/bin/env python

import json
import http.server
import os
import serial
import socketserver
import sys
import time

DEBUG = 0

class Serial:

    def __init__(self, port):
        self.ser = serial.Serial(port, 115200)
        time.sleep(1)
        print("Talking to " + self.ser.name)
        self.ack()
        print("Communication acknowledged.")

    def __del__(self):
        self.ser.close()

    def ack(self):
        self.send('A')
        if not self.get_response()[0]:
            print("Acknowledgment error.")
            sys.exit(1)

    def get_response(self):
        self.ser.readline()  # discard request
        r = self.ser.readline().decode('utf-8').replace('\r', '').replace('\n', '')
        if DEBUG != 0:
            print("<- " + r)
        s = r.split()
        return (s[0] == '+', list(map(lambda h: int(h, 16), s[1:])))

    def send(self, cmd, pars=[]):
        req = cmd + ' ' + ' '.join(map(lambda v: '%x' % v, pars))
        if DEBUG != 0:
            print("-> " + req)
        self.ser.write(bytes(req + '\n', 'utf-8'))

    def memory_page(self, page):
        self.send('R', [page * 0x100, 256])
        ok, data = self.get_response()
        print(data)
        return data[1:] if ok else None

    def memory_set(self, address, value):
        self.send('W', [address, 1, value])
        ok, _ = self.get_response()
        return ok

class Server(http.server.SimpleHTTPRequestHandler):

    def send_object(self, obj=None):
        self.send_response(200, 'OK')
        self.end_headers()
        if obj != None:
            self.wfile.write(bytes(json.dumps(obj), 'utf-8'))

    def do_GET(self):
        path = self.path.split('?')[0]
        resource = path[1:].split('/')
        if path == '/' or path.endswith('.html') or path.endswith('.css') or path.endswith('.js'):
            super().do_GET()
        elif resource[0] == 'memory':
            page = int(resource[1])
            self.send_object(serial.memory_page(page))
        else:
            self.send_response(404, 'Not found')
            self.end_headers()
            self.wfile.write(b'404 - Not found.\n')

    def do_POST(self):
        path = self.path.split('?')[0]
        resource = path[1:].split('/')
        if resource[0] == 'memory':
            address = int(resource[1])
            value = json.loads(self.rfile.read(int(self.headers['Content-Length'])))['value']
            serial.memory_set(address, value)
            self.send_object()
        else:
            self.send_response(404, 'Not found')
            self.end_headers()
            self.wfile.write(b'404 - Not found.\n')

if len(sys.argv) != 2:
    print("Usage: " + sys.argv[0] + " SERIAL_PORT")
    sys.exit(1)

serial = Serial(sys.argv[1])

socketserver.TCPServer.allow_reuse_address = True
print("Listening on 8000...")
httpd = socketserver.TCPServer(('127.0.0.1', 8000), Server)
httpd.allow_reuse_address = True
httpd.serve_forever()
