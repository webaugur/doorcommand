import socket

UDP_IP = "10.0.0.35"
UDP_PORT = 60000
MESSAGE = '7E2DD29D100100000000000000000000000000000000000000000000000000AD010D'



sock = socket.socket(socket.AF_INET, # Internet
             socket.SOCK_DGRAM) # UDP
sock.sendto(bytes.fromhex(MESSAGE), (UDP_IP, UDP_PORT))

