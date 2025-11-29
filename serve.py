import http.server

HOST = '0.0.0.0'
PORT = 8080

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory='install/share', **kwargs)

    def end_headers(self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        http.server.SimpleHTTPRequestHandler.end_headers(self)

with http.server.HTTPServer((HOST, PORT), MyHandler) as httpd:
    print(f'Serving on http://{HOST}:{PORT}')
    httpd.serve_forever()
