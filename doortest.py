#!/usr/bin/python

import serial
import time
import struct

# S/N from board as hex
serno = 'e63a'



#! I'm using the ip protocol, section commented out for clarity
#region
# # initialization and open the port

# # possible timeout values:
# #    1. None: wait forever, block call
# #    2. 0: non-blocking mode, return immediately
# #    3. x, x is bigger than 0, float allowed, timeout block call

# ser = serial.Serial()
# ser.port = "/dev/ttyS0"
# ser.baudrate = 9600
# ser.bytesize = serial.EIGHTBITS  # number of bits per bytes
# ser.parity = serial.PARITY_NONE  # set parity check: no parity
# ser.stopbits = serial.STOPBITS_ONE  # number of stop bits
# # ser.timeout      = None          #block read
# ser.timeout = 1  # non-block read
# # ser.timeout      = 2              #timeout block read
# ser.xonxoff = False  # disable software flow control
# ser.rtscts = False  # disable hardware (RTS/CTS) flow control
# ser.dsrdtr = False  # disable hardware (DSR/DTR) flow control
# ser.writeTimeout = 2  # timeout for write


# # open serial port and communicate
# ser.open()
# if ser.isOpen():
#     ser.flushInput()  # flush input buffer, discarding all its contents
#     ser.flushOutput()  # flush output buffer, aborting current output
#     # and discard all that is in buffer

#     # checksum algorithm
#     csum = 0
#     cmd_bytes = [DOOR_PAYLOAD[i:i + 2] for i in range(0, len(DOOR_PAYLOAD), 2)]

#     for i in cmd_bytes:
#         csum = csum + int.from_bytes(bytes.fromhex(i), byteorder='little')

#     csum = bytes.hex(csum.to_bytes(2, byteorder='little'))
#     print("checksum: " + csum)
#     # write data
#     FRAME = '7E' + DOOR_PAYLOAD + csum + '0D'
#     print("l: " + str(len(FRAME)))
#     print("w: " + FRAME)
#     ser.write(bytes.fromhex(FRAME))

#     time.sleep(0.5)  # give the serial port sometime to receive the data

#     numOfLines = 0

#     while True:
#         response = ser.readline()
#         print("r: " + bytes.hex(response))
#         numOfLines = numOfLines + 1
#         if (numOfLines >= 5):
#             break

#     ser.close()

# else:
#     print("cannot open serial port ")
#endregion


#FIXME: Several functions have hardcoded or unique non-passed variables that need to be accounted for.
#TODO: test each of these to ensure they work properly
class Controller:
    
    def __init__(self, serial_number, address_byte):
        self.serial_number = serial_number
        self.address_byte = address_byte

    def amend_directive(self):
        local_vars = locals()
        del local_vars['self']
        function_bytes = 'F410'
        
        return self._construct_command(local_vars, function_bytes)

    def read_command(self):
        local_vars = locals()
        del local_vars['self']
        function_bytes = 'F110'
        
        return self._construct_command(local_vars, function_bytes)

    def operation_status(self, record_bytes):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '8110'

        return self._construct_command(local_vars, function_bytes)

    def l_door_open(self, door_num):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '9D10'

        return self._construct_command(local_vars, function_bytes)

    def set_time(self, ymdhms):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '8B10'

        return self._construct_command(local_vars, function_bytes)

    def set_door_control_param(self, door_num, mode, delay):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '8F10'

        return self._construct_command(local_vars, function_bytes)

    def clear_domain(self):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '9310'

        return self._construct_command(local_vars, function_bytes)

    def read_domain(self, perm_index):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '9510'

        return self._construct_command(local_vars, function_bytes)

    def tail_plus_permissions(self, index, card_id, user_num, start_ym, end_ym, time, password, standby):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '9B10'

        return self._construct_command(local_vars, function_bytes)

    def modify_permissions(self, card_id, user_num, door_num, start_ym, send_ym, time, password, standby):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '0711'

        return self._construct_command(local_vars, function_bytes)

    def delete_an_authority(self, card_id, user_num, door_num, start_ym, send_ym, time, password, standby):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '0811'

        return self._construct_command(local_vars, function_bytes)

    def read_control_time(self, time_index):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '9610'

        return self._construct_command(local_vars, function_bytes)

    def modificaiton_time(self, time_index, week_control, link_period, standby1, start_hms1, end_hms1, start_hms2, end_hsm2, start_hms3, end_hsm3, start_ymd, end_ymd, standby2):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '9710'

        return self._construct_command(local_vars, function_bytes)

    def read_record(self, record_index):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '8D10'

        return self._construct_command(local_vars, function_bytes)

    def remove_record(self, quantity_to_clear):
        local_vars = locals()
        del local_vars['self']
        function_bytes = '8E10'

        return self._construct_command(local_vars, function_bytes)

    #? local_vars is order specific. Kwargs might be way to further simplify?
    def _construct_command(self, local_vars, function_bytes):
        frame = self.serial_number + function_bytes + ''.join(local_vars.values())
        frame = self._zero_fill_frame(frame)
        checksum = self._checksum(frame)
        return '7E'+frame+checksum+'0D'

    def _zero_fill_frame(self, unf_frame):
        frame_length = 60
        return unf_frame+''.zfill(frame_length-len(unf_frame))

    def _checksum(self, frame):
        checksum = sum(bytearray.fromhex(frame)).to_bytes(
            2, byteorder='little')
        return bytes.hex(checksum)

#TODO: there's more comamnds to add, but I don't know how worth it is to write them out if we don't need them



# test code left intentionally
c = Controller(serno, '00')
door_num = '00'
mode = '00'
delay = '00'
print(c.set_door_control_param(door_num, mode, delay))