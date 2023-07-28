#!/usr/bin/env python

import json
import http.server
import socketserver

class Serial:

    def memory_page(self, page):
        return 256 * [0x65]

class Server(http.server.SimpleHTTPRequestHandler):

    def send_object(self, obj):
        self.send_response(200, 'OK')
        self.end_headers()
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


serial = Serial()

socketserver.TCPServer.allow_reuse_address = True
httpd = socketserver.TCPServer(('127.0.0.1', 8000), Server)
httpd.allow_reuse_address = True
httpd.serve_forever()
