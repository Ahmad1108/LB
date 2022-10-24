
from socket import * 

try:

    with open('http_port','r') as f:
        s= f.read()
        httpPort=int(s)
        
except IOError:
    print("Error:can't find or read data")
        

s=socket()
s.connect(('127.0.0.1', httpPort))
print("test.py connected to LB ")
s.send("""GET /counter HTTP/1.1\r\nHost: nova.cs.tau.ac.il\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko)Chrome/31.0.1650.57 Safari/537.36\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Language: en-US,en;q=0.8,he;q=0.6\r\n\r\n""")
final = s.recv(1024)
print(final)