# doorcommand
Command Line Door Control Application for NORDSON NS-E100, NS-E200, NS-E400 door controls.
http://www.chinanordson.com/tcp-ip-network-access-control-board/

## Dependencies
You may obtain WComm_UDP.dll, WComm_UDP.tlb, protocol documentation, and related SDK from 
http://www.chinanordson.com/download.html
* NS-E Series Access Control Software
* NS-E Series Access Control Panel SDK

## Known Working non-Nordson boards
* Unnamed single door board marked with SK900A9010097 barcode. Silkscreen states "2015-01-01 v9901"

## Notes

### Serial Port Access without LAN, a dire warning
tldr; Dont connect your doors to your real local network. You don't need uplinks. Use a dedicated programming port on your door network with no LAN access, no uplinks, no gateway, etc. Use VLANs at the switches if you must share switches with your real LAN. Hire an expert if you aren't sure. These are your doors, afterall.

To be clear, if you can access the Ethernet these things are on you can download the entire employee directory, door auth, schedules, etc and manipulate it any way you want then upload it back to the door with not even a log entry stating you did anything in most cases. 

There is a WComm_Serial.dll and a wrapper for the UDP library that makes it emulate the serial version.  Some/all of these boards will have some kind of serial port exposed via the USART on the atmega128A MCU.  The v9901 board has an unpopulated header next to the MCU with 5 pins. 
* 1 - RXD, MCU Pin 2
* 2 - Ground, MCU Pin 53, 63
* 3 - (reset???)
* 4 - SCK, MCU Pin 11, Clock, unused for "RS-485/232" UART mode
* 5 - TXD, MCU Pin 3

The Ethernet implementation basically emulates RS-485 serial bus (logic analyzer modbus analysis module decodes it with some checksum errors) over Ethernet using telnet over broadcast IPs (224.0.0.0?) based on the serial number. The full serial data protocol is described in the SDK documentation from Nordson (above). No security, no obfuscation, passwords are on the software not the hardware, etc. Basically if you know the serial number and can plugin to the door control LAN you own the door. Also, there are no apparent rate limits so you can scan the entire range of serial numbers in minutes and just open all the doors in a building in one blast. Nonetheless, that seeming horror is really quite fine if you just secure your security system physical wiring on the secure side of things and inside steel tubing and walls. You know, don't drape the door control ethernet cable over the insecure hallway.

### What I would do differently
And this is still an option for future...  Leave the door control board the way it is. But replace the Ethernet port with something like an Allwinner H3 based Linux system, with optional audio and video capabilities, that is running crypto based authentication and protocol encryption. Could be as simple as just using SSH with certs and keep the telnet like interface so you can swap out a DLL and use your old software. Old school, set your default shell on the admin account to be a pipe to the serial port. Let the firmware think you are still using telnet. Implement ONVIF Profile-A on the Linux system to allow it to interoperate with more recent security systems without the need for special/proprietary software.
