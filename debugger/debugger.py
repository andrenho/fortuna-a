#!/usr/bin/env python

import argparse
import json
import http.server
import os
import platform
import serial
import socketserver
import subprocess
import sys
import time

DEBUG = 0

#################
#               #
#   COMPILER    #
#               #
#################

def compile(source_filename):
    exe = './vasmz80_oldstyle'
    if platform.system() == 'Windows':
        exe += '.exe'
    if platform.system() == 'Darwin':
        exe += '_macos'
    cp = subprocess.run([exe, '-chklabels', '-L', 'listing.txt', '-Llo', '-Lns', '-ignore-mult-inc', '-nosym', '-x', '-Fbin', '-o', 'rom.bin', source_filename], capture_output=True, text=True)

    if cp.returncode != 0:
        return { 'stderr': cp.stderr, 'status': cp.returncode }

    dbg_source = ''
    rom = None
    with open('listing.txt', 'r') as f:
        dbg_source = f.read()
    if os.path.exists('listing.txt'):
        os.remove('listing.txt')
    if os.path.exists('rom.bin'):
        with open('rom.bin', 'rb') as f:
            rom = [x for x in bytearray(f.read())]
        os.remove('rom.bin')
    return { 'src': dbg_source, 'rom': rom, 'stdout': cp.stdout, 'stderr': cp.stderr, 'status': cp.returncode }

#################
#               #
#    SERIAL     #
#               #
#################

postTests = {
    'a': 'Read ROM memory',
    'b': 'Read shared memory',
    'c': 'Read high memory',
    'd': 'Write ROM memory',
    'e': 'Write shared memory',
    'f': 'Write high memory',
    'g': 'Write memory banks',
    'h': 'Write RAMONLY',
}

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

    def get_response(self, convert_to_int=True):
        self.ser.readline()  # discard request
        r = self.ser.readline().decode('utf-8').replace('\r', '').replace('\n', '')
        if DEBUG != 0:
            print("<- " + r)
        s = r.split()
        if convert_to_int:
            return (s[0] == '+', list(map(lambda h: int(h, 16), s[1:])))
        else:
            return (s[0] == '+', s[1:])

    def send(self, cmd, pars=[]):
        req = cmd + ' ' + ' '.join(map(lambda v: '%x' % v, pars))
        if DEBUG != 0:
            print("-> " + req)
        self.ser.write(bytes(req + '\n', 'utf-8'))

    def memory_page(self, page):
        self.send('R', [page * 0x100, 256])
        ok, data = self.get_response()
        return data[1:] if ok else None

    def memory_set(self, address, data):
        self.send('W', [address, len(data)] + data)
        ok, _ = self.get_response()
        return ok

    def self_test(self):
        self.send('P')
        ok, r = self.get_response(False)
        return list(map(lambda m: { 'test': postTests[m[1]], 'result': m[0] == '+' }, r))

    def step_cycle(self):
        self.send('s')
        ok, r = self.get_response()
        data, addr, m1, iorq, busak, wait, int_, wr, rd, mreq = r
        return {
            'data': data if mreq == 0 else None,
            'addr': addr if mreq == 0 else None,
            'm1': m1 == 1,
            'iorq': iorq == 1,
            'busak': busak == 1,
            'wait': wait == 1,
            'int': int_ == 1,
            'wr': wr == 1,
            'rd': rd == 1,
            'mreq': mreq == 1
        }

    def step(self):
        self.send('S')
        ok, r = self.get_response()
        return {
            'pc': r[0]  # TODO
        }

    def reset(self):
        self.send('X')
        ok, _ = self.get_response()
        return ok

#################
#               #
#  HTTP SERVER  #
#               #
#################

class Server(http.server.SimpleHTTPRequestHandler):

    def send_object(self, obj=None, success=True):
        if success:
            self.send_response(200, 'OK')
        else:
            self.send_response(500, 'Server error')
        self.end_headers()
        if obj == None:
            obj = {}
        self.wfile.write(bytes(json.dumps(obj), 'utf-8'))

    def do_GET(self):
        path = self.path.split('?')[0]
        resource = path[1:].split('/')
        if path == '/' or path.endswith('.html') or path.endswith('.css') or path.endswith('.js'):
            super().do_GET()
        elif resource[0] == 'memory':
            page = int(resource[1])
            self.send_object(serial.memory_page(page))
        elif resource[0] == 'code':
            r = compile(args.source)
            self.send_object(r, r['status'] == 0)
        else:
            self.send_response(404, 'Not found')
            self.end_headers()
            self.wfile.write(b'404 - Not found.\n')

    def do_POST(self):
        path = self.path.split('?')[0]
        resource = path[1:].split('/')
        if resource[0] == 'memory':
            address = int(resource[1])
            data = json.loads(self.rfile.read(int(self.headers['Content-Length'])))['data']
            serial.memory_set(address, data)
            self.send_object()
        elif resource[0] == 'post':
            self.send_object(serial.self_test())
        elif resource[0] == 'step-cycle':
            self.send_object(serial.step_cycle())
        elif resource[0] == 'step':
            self.send_object(serial.step())
        elif resource[0] == 'reset':
            serial.reset()
            self.send_object()
        else:
            self.send_response(404, 'Not found')
            self.end_headers()
            self.wfile.write(b'404 - Not found.\n')

parser = argparse.ArgumentParser()
parser.add_argument('source')
parser.add_argument('-p', '--serial-port', required=True)
parser.add_argument('-l', '--log', action='store_true')
args = parser.parse_args()

if args.log:
    DEBUG = 1

serial = Serial(args.serial_port)

socketserver.TCPServer.allow_reuse_address = True
print("Listening on 8000...")
httpd = socketserver.TCPServer(('127.0.0.1', 8000), Server)
httpd.allow_reuse_address = True
httpd.serve_forever()
