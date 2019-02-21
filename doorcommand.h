/* DoorCommand - Managed Object Interface for Door Access Controllers
 * 
 * Copyright 2019 David L Norris <dave@webaugur.com>
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list 
 * of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this 
 * list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 */
#ifndef DOORCOMMAND_H
#define DOORCOMMAND_H

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "_vcclrit.h"

#using "WComm_UDP.dll"
#using <mscorlib.dll>
using namespace System;

/*!
    \brief Door Command Object

    Door Command Interface Definition.
*/
__gc class DoorCommand
{
public:
    DoorCommand();
    DoorCommand* setSerial(long serial);
    long getSerial();
    DoorCommand* setCommand(String* command);
    String* getCommand();
    DoorCommand* setResult(String* result);
    String* getResult();
    DoorCommand* setIPAddress(String* ip_address);
    String* getIPAddress();
    DoorCommand* setIPPort(int ip_port);
    int getIPPort();
    DoorCommand* setStatus(__int64 status);
    __int64 getStatus();
    DoorCommand* setDateTime(String* date_time);
    String* getDateTime();
    DoorCommand* setValidCardCount(__int64 valid_card_count);
    __int64 getValidCardCount();
    DoorCommand* executeCommand(String* command);
    DoorCommand* doorRunInfo();
    DoorCommand* doorOpen(int number_door);

private:
    static DoorCommand          *_instance      = NULL; //!< instance of _self
    WComm_UDP::IWCOMM_OPERATE   *_connection;       //!< wudp - .NET UDP object
    long                        _serial;            //!< controllerSN - Board Serial Number
    String*                     _command;           //!< strCmd - Command, Serial Data Frame
    String*                     _result;            //!< strFrame - Result of Command, Serial Data Frame
    int                         _ip_port;           //!< ipPort - Controller TCP Port
    String*                     _ip_address;        //!< ipAddr - Controller IP Address
    __int64                     _status;            //!< status - Status     
    __int64                     _valid_card_count;       //!< Total count of valid swipes
    __int64                     _permission_total;  //!< Total number of permission records
    String*                     _date_time;         //!< Date and Time from Door Controller
};

#endif