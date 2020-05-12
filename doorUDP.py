import socket
import sys

HOST, PORT = "localhost", 9998
data = "This is test data"


# Create a socket (SOCK_STREAM means a TCP socket)
with socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    
    sock.bind(('', 6969))
    # sock.connect(('192.168.1.221', 6969))
    sock.sendto(bytes(data, 'utf-8'), ('255.255.255.255', 60000))

    # Receive data from the server and shut down
    received = str(sock.recv(1024), "utf-8")

print("Sent:     {}".format(data))
print("Received: {}".format(received))