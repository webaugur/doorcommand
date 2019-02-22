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

#include "stdafx.h"
#include <windows.h>
#include <vcclr.h>
#include <string>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "tchar.h"
#include "doorcommand.h"
using namespace System;

    /*!< /brief DoorCommand constructor
        
        DoorCommand is arranged as a SingleTon pattern. 
        Method chaining is available for many operations.
        */
    DoorCommand::DoorCommand(){
        // new object needs to be initialized
        if(!_instance){
            _connection = new WComm_UDP::WComm_Operate();
            _serial = 0;
            _command = "";
            _result = "";
            _ip_port = 60000;
            _ip_address = ""; // If IP Address is Empty String Broadcast Mode is enabled
            _status = 0;
            _instance = this;
        } 
        // existing object just needs to be returned
        else {
                _instance = this;
        }
    }

    /*!
        \brief Set Serial Number of Door

        This serial number is used to communicate with the desired door controller.
        This must be the FIRST method called on this object before communication can occur.
        This is literally the only access token required. You don't even need to know
        the IP address of the device. This function is not likely to be called publicly
        but there is no reason, currently, not to do so.
    */
    DoorCommand* DoorCommand::setSerial(long serial /**< [in] Door Controller Serial Number. */)
    {
        _serial = serial;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief Get Serial Number of Door

        The serial number used communicate with the desired door controller.
    */
    long DoorCommand::getSerial()
    {
        return _serial; /**< [out] Door Controller Serial Number. */
    }

    /*!
        \brief Next Queued Command

        Holds the next Command to be issued. 
        This is used mostly internally by specific methods.  
        Returns Empty String if there is no command to be executed.
    */
    DoorCommand* DoorCommand::setCommand(String* command/**< [in] Text of Command. */)
    {
        _command = command;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief Queued Command

        Contains the text of the queued command. Mostly used internally by
        other methods.
    */
    String* DoorCommand::getCommand()
    {
        return _command; /**< [out] Command to be executed (may be ""). */
    }


    /*!
        \brief Most Recent Result

        Holds the results of the most recent command
        This is used mostly internally by specific methods.  
        Returns Empty String if there is no result.
    */
    DoorCommand* DoorCommand::setResult(String* result/**< [in] Text of Result. */)
    {
        _result = result;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief Result from most recent Command

        Contains the text of the queued command. Mostly used internally by
        other methods.
    */
    String* DoorCommand::getResult()
    {
        return _result; /**< [out] Result of the most recent command. */
    }

    /*!
        \brief set IP Address

        Holds the IP Address.
        If Empty String Broadcast Mode is Used
    */
    DoorCommand* DoorCommand::setIPAddress(String* ip_address/**< [in] Text of IPv4 Address, Empty = Broadcast. */)
    {
        _ip_address = ip_address;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get IP Address

        Contains the text of the IPv4 address of this door.
    */
    String* DoorCommand::getIPAddress()
    {
        return _ip_address; /**< [out] IP Address (may be "" for broadcast). */
    }

    /*!
        \brief set IP Port

        Holds the IP Port. Must be an integer 1024-65535
    */
    DoorCommand* DoorCommand::setIPPort(int ip_port/**< [in] Text of IPv4 Port. */)
    {
        // bounds check the port number
        if( (ip_port > 1024) && (ip_port <= 65535) ){
            _ip_port = ip_port;
            return _instance; /**< [out] instance of self for method chaining. */
        }
        else {
            throw std::invalid_argument( "IP Port must be 1024-65535" );
        }
    }

    /*!
        \brief get IP Port

        Contains the IPv4 port number for this door.
    */
    int DoorCommand::getIPPort()
    {
        return _ip_port; /**< [out] IP Port, defaults to 60000. */
    }

    /*!
        \brief set Status

        Holds the Status of Most Recently Executed Command. Mostly internally use.
    */
    DoorCommand* DoorCommand::setStatus(__int64 status/**< [in] Status code. */)
    {
        _status = status;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get Status

        Gets the status of the most recently executed command
    */
    __int64 DoorCommand::getStatus()
    {
        return _status; /**< [out] Status of most recent command. */
    }



    /*!
        \brief set Date & Time

        Holds the Status of Most Recently Executed Command. Mostly internally use.
    */
    DoorCommand* DoorCommand::setDateTime(String* date_time/**< [in] Date on Board RTC. */)
    {
        _date_time = date_time;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get Date & Time

        Gets the date from the hardware
    */
    String* DoorCommand::getDateTime()
    {
        return _date_time; /**< [out] Date and Time from Controller board. */
    }




    /*!
        \brief set Valid Card Count

        Holds the number of valid card presentations
    */
    DoorCommand* DoorCommand::setValidCardCount(__int64 valid_card_count /**< [in] Total Number of Valid Cards Presented. */)
    {
        _valid_card_count = valid_card_count;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get Date & Time

        Gets the date from the hardware
    */
    __int64 DoorCommand::getValidCardCount()
    {
        return _valid_card_count; /**< [out] Date and Time from Controller board. */
    }



    /* 
    Need getters and setters for these
    __int64                     _valid_card_count;       //!< Total count of valid swipes
    __int64                     _permission_total;  //!< Total number of permission records
    */

    DoorCommand* DoorCommand::executeCommand(String* command)
    {
        
        // Set the Command
        this->setCommand ( 
            this->_connection->CreateBstrCommand(this->getSerial(), command)
        );

        // Execute the Command
        this->setResult(
            this->_connection->udp_comm(
                this->getCommand(), 
                this->getIPAddress(), 
                this->getIPPort()
            )
        );

        // If an error occurred or frame is empty 
        if((0 != this->_connection->ErrCode) || (this->getResult()->Length == 0))
        {
            throw std::invalid_argument( "UDP Communication Error" );
        }

        return this->_instance;
    }

    /*! 
        \brief Execute Command: RunInformation 

        Retrieves the runtime information from the controller

    */
    DoorCommand* DoorCommand::doorRunInfo()
    {
        this->executeCommand("811000000000");
        // set the date time from hardware
        this->setDateTime(
            this->_connection->GetClockTimeFromRunInfo(this->getResult())
        );


        this->setValidCardCount(this->_connection->GetCardRecordCountFromRunInfo(this->getResult()));

        return this->_instance;
    }

    DoorCommand* DoorCommand::doorOpen(int number_door){
        this->executeCommand("9D1001");
        return this->_instance;
    }
