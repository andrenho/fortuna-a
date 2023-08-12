#!/usr/bin/env python

import json
import http.server
import socketserver

class Serial:

    def memory_page(self, page):
        return 256 * [0x65]

    def memory_set(self, address, value):
        print(address, value)


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


serial = Serial()

socketserver.TCPServer.allow_reuse_address = True
print("Listening on 8000...")
httpd = socketserver.TCPServer(('127.0.0.1', 8000), Server)
httpd.allow_reuse_address = True
httpd.serve_forever()
