#!/usr/bin/python

import serial, time,struct

# serial number
serno = '2dd2'


# Commands
# Open Door (9D10) - 01 - unused - checksum - eol 
DOOR_OPEN           = '9D10'+'01'+'00000000000000000000000000000000000000000000000000'#+'AD01'
# Door Config (108F) - Door 01 - Mode 01 - Delay 3 second 
DOOR_LATCH_OPEN     = '8F10'+'01'+'01'+'1E00'+'00000000000000000000000000000000000000000000'
# Door Config (108F) - Door 01 - Mode 02 - Delay 3 second 
DOOR_LATCH_CLOSED   = '8F10'+'01'+'02'+'1E00'+'00000000000000000000000000000000000000000000'
# Door Config (108F) - Door 01 - Mode 02 - Delay 3 second 
DOOR_ONLINE         = '8F10'+'01'+'03'+'1E00'+'00000000000000000000000000000000000000000000'

DOOR_STATUS         = '8110'+'00000000'+'00000000000000000000000000000000000000000000'


DOOR_TEST = DOOR_ONLINE
DOOR_PAYLOAD = serno + DOOR_TEST

#initialization and open the port

#possible timeout values:
#    1. None: wait forever, block call
#    2. 0: non-blocking mode, return immediately
#    3. x, x is bigger than 0, float allowed, timeout block call

ser               = serial.Serial()
ser.port          = "/dev/tty.usbserial"
ser.baudrate      = 9600
ser.bytesize      = serial.EIGHTBITS #number of bits per bytes
ser.parity        = serial.PARITY_NONE #set parity check: no parity
ser.stopbits      = serial.STOPBITS_ONE #number of stop bits
#ser.timeout      = None          #block read
ser.timeout       = 1            #non-block read
#ser.timeout      = 2              #timeout block read
ser.xonxoff       = False     #disable software flow control
ser.rtscts        = False     #disable hardware (RTS/CTS) flow control
ser.dsrdtr        = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout  = 2     #timeout for write


# open serial port and communicate
ser.open()
if ser.isOpen():
    ser.flushInput() #flush input buffer, discarding all its contents
    ser.flushOutput()#flush output buffer, aborting current output 
                 #and discard all that is in buffer


    # checksum algorithm
    csum = 0
    cmd_bytes = [DOOR_PAYLOAD[i:i + 2] for i in range(0, len(DOOR_PAYLOAD), 2)]

    for i in cmd_bytes:
        csum = csum + int.from_bytes(bytes.fromhex(i), byteorder='little')
    
    csum = bytes.hex(csum.to_bytes(2, byteorder='little'))
    print("checksum: " + csum)




    #write data
    FRAME = '7E' + DOOR_PAYLOAD + csum +'0D'
    print("l: " + str(len(FRAME)) );
    print("w: " + FRAME)
    ser.write(bytes.fromhex(FRAME))

    time.sleep(0.5)  #give the serial port sometime to receive the data

    numOfLines = 0

    while True:
        response = ser.readline()
        print("r: " + bytes.hex(response))
        numOfLines = numOfLines + 1
        if (numOfLines >= 5):
            break

    ser.close()

else:
    print("cannot open serial port ")