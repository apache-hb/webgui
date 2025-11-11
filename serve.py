import http.server
import ssl

HOST = '0.0.0.0'
PORT = 4443
Handler = http.server.SimpleHTTPRequestHandler

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        http.server.SimpleHTTPRequestHandler.end_headers(self)

with http.server.HTTPServer((HOST, PORT), MyHandler) as httpd:
    print("Web Server listening at => " + HOST + ":" + str(PORT))
    sslcontext = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    sslcontext.load_cert_chain(keyfile="local.key", certfile="local.crt")
    httpd.socket = sslcontext.wrap_socket(httpd.socket, server_side=True)
    httpd.serve_forever()
