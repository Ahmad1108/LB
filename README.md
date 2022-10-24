# LOAD BALANCING

This project is about to demonstrate a simple version of Load Balancer(LB).
The LB is written in C, the server is written in python.

LB guesses a server port to let computers connect to it and also a http port to connect the browser to it.

the browser (not yet implemented) connects to the LB with http port and it send a HTTP request to LB.
Three computers connect to LB with server port and wait for LB to accept.
The LB send the HTTP request to one of the computers in cyclic order and wait for HTTP response.
