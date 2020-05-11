#!/usr/bin/python

import serial
import time
import struct

# serial number
# serno = '2dd2'
serno = 'e63a'


# Commands
# Open Door (9D10) - 01 - unused - checksum - eol
DOOR_OPEN = '9D10'+'01'+'00000000000000000000000000000000000000000000000000'  # +'AD01'
# Door Config (108F) - Door 01 - Mode 01 - Delay 3 second
DOOR_LATCH_OPEN = '8F10'+'01'+'01'+'1E00' + \
    '00000000000000000000000000000000000000000000'
# Door Config (108F) - Door 01 - Mode 02 - Delay 3 second
DOOR_LATCH_CLOSED = '8F10'+'01'+'02'+'1E00' + \
    '00000000000000000000000000000000000000000000'
# Door Config (108F) - Door 01 - Mode 02 - Delay 3 second
DOOR_ONLINE = '8F10'+'01'+'03'+'1E00' + \
    '00000000000000000000000000000000000000000000'

DOOR_STATUS = '8110'+'00000000'+'00000000000000000000000000000000000000000000'


DOOR_TEST = DOOR_ONLINE
DOOR_PAYLOAD = serno + DOOR_TEST

# initialization and open the port

# possible timeout values:
#    1. None: wait forever, block call
#    2. 0: non-blocking mode, return immediately
#    3. x, x is bigger than 0, float allowed, timeout block call

ser = serial.Serial()
ser.port = "/dev/tty.usbserial"
ser.baudrate = 9600
ser.bytesize = serial.EIGHTBITS  # number of bits per bytes
ser.parity = serial.PARITY_NONE  # set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE  # number of stop bits
# ser.timeout      = None          #block read
ser.timeout = 1  # non-block read
# ser.timeout      = 2              #timeout block read
ser.xonxoff = False  # disable software flow control
ser.rtscts = False  # disable hardware (RTS/CTS) flow control
ser.dsrdtr = False  # disable hardware (DSR/DTR) flow control
ser.writeTimeout = 2  # timeout for write


# open serial port and communicate
ser.open()
if ser.isOpen():
    ser.flushInput()  # flush input buffer, discarding all its contents
    ser.flushOutput()  # flush output buffer, aborting current output
    # and discard all that is in buffer

    # checksum algorithm
    csum = 0
    cmd_bytes = [DOOR_PAYLOAD[i:i + 2] for i in range(0, len(DOOR_PAYLOAD), 2)]

    for i in cmd_bytes:
        csum = csum + int.from_bytes(bytes.fromhex(i), byteorder='little')

    csum = bytes.hex(csum.to_bytes(2, byteorder='little'))
    print("checksum: " + csum)
    # write data
    FRAME = '7E' + DOOR_PAYLOAD + csum + '0D'
    print("l: " + str(len(FRAME)))
    print("w: " + FRAME)
    ser.write(bytes.fromhex(FRAME))

    time.sleep(0.5)  # give the serial port sometime to receive the data

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


#Most of these commands need tweaked based on how they actually work and what needs to be provided. Eg in amend_directive i believe the '0003' can be any number? 
class Controller:
    def __init__(self, serial_number, address_byte):
        self.serial_number = serial_number
        self.address_byte = address_byte

    def amend_directive(self):
        function_bytes = 'F410'
        frame = self.serial_number + function_bytes + \
            self.address_byte + '0003'
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def read_command(self):
        function_bytes = 'F110'
        frame = self.serial_number + function_bytes + \
            self.address_byte
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def operation_status(self, record_bytes):
        function_bytes = '8110'
        frame = self.serial_number + function_bytes + \
            record_bytes #record should be 4 bytes long
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def l_door_open(self, door_num):
        function_bytes = '9D10'
        frame = self.serial_number + function_bytes + \
            door_num
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def set_time(self, ymdhms):
        function_bytes = '8B10'
        frame = self.serial_number + function_bytes + \
            ymdhms #6 bytes 1 byte each
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def set_door_control_param(self, door_num, mode, delay):
        function_bytes = '8F10'
        frame = self.serial_number + function_bytes + \
            door_num + mode + delay
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def clear_domain(self):
        function_bytes = '9310'
        frame = self.serial_number + function_bytes
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def read_domain(self, perm_index):
        function_bytes = '9510'
        frame = self.serial_number + function_bytes + \
            perm_index
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    #TODO: convert all these to one object
    def tail_plus_permissions(self, index, card_id, user_num, start_ym, end_ym, time, password, standby):
        function_bytes = '9B10'
        frame = self.serial_number + function_bytes + \
        index + card_id + user_num + start_ym + \
        end_ym + time + password + standby
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def modify_permissions(self, card_id, user_num, door_num, start_ym, send_ym, time, password, standby):
        function_bytes = '0711'
        frame = self.serial_number + function_bytes + \
        '0000' + card_id + user_num + door_num + \
        start_ym + send_ym + time + password + standby
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def delete_an_authority(self, card_id, user_num, door_num, start_ym, send_ym, time, password, standby):
        function_bytes = '0811'
        frame = self.serial_number + function_bytes + \
        '0000' + card_id + user_num + door_num + \
        start_ym + send_ym + time + password + standby
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def read_control_time(self, time_index):
        function_bytes = '9610'
        frame = self.serial_number + function_bytes + \
        time_index
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    #What even are these descriptions of bytes.
    #I've already taken some pretty liberal liberties with names but cmon 
    def modificaiton_time(self, time_index, week_control, link_period, standby1, start_hms1, end_hms1, start_hms2, end_hsm2, start_hms3, end_hsm3, start_ymd, end_ymd, standby2):
        function_bytes = '9710'
        frame = self.serial_number + function_bytes + \
        time_index + week_control + link_period + \
        standby1 + start_hms1 + end_hms1 + start_hms2 + \
        end_hsm2 + start_hms3 + end_hsm3 + start_ymd + end_ymd + standby2
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def read_record(self, record_index):
        function_bytes = '8D10'
        frame = self.serial_number + function_bytes + \
            record_index
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def remove_record(self, quantity_to_clear):
        function_bytes = '8E10'
        frame = self.serial_number + function_bytes + \
            quantity_to_clear
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return self._construct_command(frame, checksum)

    def _construct_command(self, frame, checksum):
        return '7E'+frame+checksum+'0D'
        
    def _zero_fill_frame(self, unf_frame):
        frame_length = 60
        return unf_frame+''.zfill(frame_length-len(unf_frame))

    def _checksum(self, frame):
        checksum = sum(bytearray.fromhex(frame)).to_bytes(
            2, byteorder='little')
        return bytes.hex(checksum)
#TODO: there's more comamnds to add, but I don't know how worth it is to write them out if we don't need them