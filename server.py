#!/usr/bin/python2.7 -tt
from socket import *
import os


def connect_to_lb(server):

    try:
        with open('server_port', 'r') as f:
            s = f.read()
            serverPort = int(s)

    except IOError:
        print("Error:can't find or read data")

    # Port number of the server
    print(serverPort)
    server.connect(("127.0.0.1", serverPort))
    return server


def recieve_and_send_ans(server):

    counter = 0
    full_data = ""
    data = ""
    while counter != 1:

        data = server.recv(1024)
        if data.find('\r\n\r\n') != -1:
            counter += 1

        full_data += data

    print(full_data)

    if full_data.find('GET', 0, 3) == -1:
        server.send('HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-length: 113\r\n<html><head><title>Not Found</title></head><body>\r\nSorry, the object you requested was not found.\r\n</body></html>\r\n')

    first = 4
    end = full_data.find('HTTP')
    addr = full_data[first:end-1]
    print(addr)
    print('len of addr is: ', len(addr))

    answer = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
    answer += str(len(addr))
    answer += "r\n\r\n"
    answer += addr
    answer += "\r\n\r\n"

    server.send(answer)


server_socket1 = socket()
server_socket2 = socket()
server_socket3 = socket()

connect_to_lb(server_socket1)
connect_to_lb(server_socket2)
connect_to_lb(server_socket3)

while 1:

    recieve_and_send_ans(server_socket1)

    recieve_and_send_ans(server_socket2)

    recieve_and_send_ans(server_socket3)
