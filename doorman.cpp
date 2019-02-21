/* DoorCommand - Command Line Interface for Door Access Controllers
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
#include "getopt.h"

using namespace System;

/* I do declare */
void BecauseIUsedToBeOne(void);

/*!
    \brief Door Command Object

    Implemenation of the Door Command API.
*/
__gc class DoorCommand
{
public:
    /*!< /brief DoorCommand constructor
        
        DoorCommand is arranged as a SingleTon pattern. 
        Method chaining is available for many operations.
     */
    DoorCommand(){
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
    DoorCommand* setSerial(long serial /**< [in] Door Controller Serial Number. */)
    {
        _serial = serial;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief Get Serial Number of Door

        The serial number used communicate with the desired door controller.
    */
    long getSerial()
    {
        return _serial; /**< [out] Door Controller Serial Number. */
    }

    /*!
        \brief Next Queued Command

        Holds the next Command to be issued. 
        This is used mostly internally by specific methods.  
        Returns Empty String if there is no command to be executed.
    */
    DoorCommand* setCommand(String* command/**< [in] Text of Command. */)
    {
        _command = command;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief Queued Command

        Contains the text of the queued command. Mostly used internally by
        other methods.
    */
    String* getCommand()
    {
        return _command; /**< [out] Command to be executed (may be ""). */
    }


    /*!
        \brief Most Recent Result

        Holds the results of the most recent command
        This is used mostly internally by specific methods.  
        Returns Empty String if there is no result.
    */
    DoorCommand* setResult(String* result/**< [in] Text of Result. */)
    {
        _result = result;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief Result from most recent Command

        Contains the text of the queued command. Mostly used internally by
        other methods.
    */
    String* getResult()
    {
        return _result; /**< [out] Result of the most recent command. */
    }

    /*!
        \brief set IP Address

        Holds the IP Address.
        If Empty String Broadcast Mode is Used
    */
    DoorCommand* setIPAddress(String* ip_address/**< [in] Text of IPv4 Address, Empty = Broadcast. */)
    {
        _ip_address = ip_address;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get IP Address

        Contains the text of the IPv4 address of this door.
    */
    String* getIPAddress()
    {
        return _ip_address; /**< [out] IP Address (may be "" for broadcast). */
    }

    /*!
        \brief set IP Port

        Holds the IP Port. Must be an integer 1024-65535
    */
    DoorCommand* setIPPort(int ip_port/**< [in] Text of IPv4 Port. */)
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
    int getIPPort()
    {
        return _ip_port; /**< [out] IP Port, defaults to 60000. */
    }

    /*!
        \brief set Status

        Holds the Status of Most Recently Executed Command. Mostly internally use.
    */
    DoorCommand* setStatus(__int64 status/**< [in] Status code. */)
    {
        _status = status;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get Status

        Gets the status of the most recently executed command
    */
    __int64 getStatus()
    {
        return _status; /**< [out] Status of most recent command. */
    }

    

    /*!
        \brief set Date & Time

        Holds the Status of Most Recently Executed Command. Mostly internally use.
    */
    DoorCommand* setDateTime(String* date_time/**< [in] Date on Board RTC. */)
    {
        _date_time = date_time;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get Date & Time

        Gets the date from the hardware
    */
    String* getDateTime()
    {
        return _date_time; /**< [out] Date and Time from Controller board. */
    }


  

    /*!
        \brief set Valid Card Count

        Holds the number of valid card presentations
    */
    DoorCommand* setValidCardCount(__int64 valid_card_count /**< [in] Total Number of Valid Cards Presented. */)
    {
        _valid_card_count = valid_card_count;
        return _instance; /**< [out] instance of self for method chaining. */
    }

    /*!
        \brief get Date & Time

        Gets the date from the hardware
    */
    __int64 getValidCardCount()
    {
        return _valid_card_count; /**< [out] Date and Time from Controller board. */
    }



/* 
    Need getters and setters for these
    __int64                     _valid_card_count;       //!< Total count of valid swipes
    __int64                     _permission_total;  //!< Total number of permission records
*/

    DoorCommand* executeCommand(String* command)
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
    DoorCommand* doorRunInfo()
    {
        this->executeCommand("811000000000");
        // set the date time from hardware
        this->setDateTime(
            this->_connection->GetClockTimeFromRunInfo(this->getResult())
        );


        this->setValidCardCount(this->_connection->GetCardRecordCountFromRunInfo(this->getResult()));

        return this->_instance;
    }

    DoorCommand* doorOpen(int number_door){
        this->executeCommand("9D1001");
        return this->_instance;
    }


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

void printValidCardCount(DoorCommand* command);
void printDateTime(DoorCommand* command);

void openDoors(DoorCommand* command, int door_numbers);

int _tmain (int argc, char **argv)
{
  int dflag = 0;
  int vflag = 0;
  int oflag = 0;
  int doors = 0;
  int index;
  int c;

  opterr = 0;

   /* Do Door Stuff. Be OutDoorsy. Get Your Door On. */
   DoorCommand* command = new DoorCommand();

   while ((c = getopt (argc, argv, "a:s:o:p:dv")) != -1)
    switch (c)
      {
      case 'a':
            command->setIPAddress(optarg); // set IP address
        break;
      case 'p':
            command->setIPPort(atoi(optarg)); // set IP Port
        break;
      case 's':
            command->setSerial(atoi(optarg)); // set Serial Number
        break;
      case 'o':
            doors = atoi(optarg);
            oflag = 1; // open doors
        break;
      case 'd':
            dflag = 1;
        break;
      case 'v':
            vflag = 1;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);


   // print date and time
   if(dflag){
        printDateTime(command);
   }

   // print the valid card count
   if(dflag){
        printValidCardCount(command);
   }

   if(oflag){
        openDoors(command, doors);
   }

    /* Still doing all the things we used to do */
    //BecauseIUsedToBeOne();

    return 0;
}

/*
    Print Date and Time from Controller
*/
void printDateTime(DoorCommand* command){
    Console::WriteLine("date_time: {0}", command->doorRunInfo()->getDateTime()); // print controller date-time
}

/* 
    \brief Print Valid Card Count from Controller

    This is a count of the total number of cards or keypad 
    entries presented to the reader since log records were last erased.

*/
void printValidCardCount(DoorCommand* command){
    Console::WriteLine("valid_card_count: {0}", command->doorRunInfo()->getValidCardCount().ToString()); // print controller valid card count
}

void openDoors(DoorCommand* command, int door_numbers){
    Console::WriteLine("opening_doors: {0}", door_numbers.ToString()); // Open doors
    command->doorOpen(door_numbers);
}


/* I don't do main() because I used to be one */
  void BecauseIUsedToBeOne(void){
    WComm_UDP::IWCOMM_OPERATE        *wudp;
    wudp = new WComm_UDP::WComm_Operate();

    String* strTemp;
    String* strFrame;      // Return Data
    String* strCmd;       // To send the command frame
    __int64 controllerSN; // Controller Series No.
    int ipPort;       // Controller TCP Port
    String* ipAddr;          // IP Address
                  
    // Variable card records
    __int64  cardId;        //Card Number 
    __int64  status;        //Status     
    String* strSwipeDate;   //Date time

    // constant variables?
    controllerSN = 53805;
    ipAddr = "10.0.0.35";
    ipPort = 60000;
                
    //read the run status information
    strCmd = wudp->CreateBstrCommand(controllerSN, "811000000000");    
    //Command frame generated wudp.NumToStrHex(0,3) That 0 records,                     
    //it records the latest 
    strFrame = wudp -> udp_comm(strCmd, ipAddr, ipPort);    //send command and get back information
    if (( 0 != wudp ->ErrCode)||(strFrame->Length  ==0))    
    {
        Console::WriteLine("Error Communicating via UDP Broadcast");
                        
        exit(1);
        //Did not got data,
        //Failure handling code ... (check ErrCode targeted analytical processing)
    }
                    
    // Success presumably???
    Console::WriteLine("Successfully obtained controller run info");

    // Device Serial Number from device run info
    Console::WriteLine("Device Series S/N: {0}", wudp->GetSNFromRunInfo(strFrame).ToString());

    Console::WriteLine("Clock: {0}", wudp->GetClockTimeFromRunInfo(strFrame));
    Console::WriteLine("Valid Card Count: ", wudp->GetCardRecordCountFromRunInfo(strFrame).ToString());
    Console::WriteLine("Number of permissions: ", wudp->GetPrivilegeNumFromRunInfo(strFrame).ToString());

    strSwipeDate = wudp->GetSwipeDateFromRunInfo(strFrame,  &cardId, &status) ;
    if (strSwipeDate->Length != 0 )
    {
        // Detailed analysis of operational information
        //controller of the current time
        Console::WriteLine( 
            "Most recent card read: (number, status, statusid, time) = ({0},{1},{2},{3})",
            cardId.ToString(),
            status.ToString(),
            wudp->NumToStrHex(status,1),
            strSwipeDate
        );
    }

    //remoting to open the Door NO.1 
    strCmd = wudp->CreateBstrCommand(controllerSN,"9D1001");  //Command frame generated
    strFrame = wudp -> udp_comm(strCmd, ipAddr, ipPort);                        //Send command, and get back information
    if (( 0 != wudp ->ErrCode)||(strFrame->Length  ==0))    
    {
        Console::WriteLine("Error remotely opening door #1");
        exit(2);
        //Not received data,
        //failure handling code ... (check ErrCode targeted analytical processing)
    }
    else
    {
        Console::WriteLine("Successfully opened door");
    }

    //Real-time monitoring
    //read to run state is the key to achieving control instructions. Conducting surveillance, the first record of the index to read the                     //latest bit of records. Read the                     //latest record, while the number of records can get to the credit card.
    //Then you can use to read the card plus 1 to fill the index to read the latest position on the record, to read the running to get the                     //next record.
    //If you read into a new credit card records, it can be add index wth 1 bit, otherwise the bit to keep the index unchanged. This allows                     //real-time monitoring data.
    //face communication is not the treatment, serial communication can take this time out as errors of 400-500 milliseconds, can try again,                     //and then not receive data, can be                     //considered between device and PC, can not communicate.
    __int64 watchIndex;
    long recCnt;

    watchIndex = 0   ;                   //default from 0, indicating a recent record of the first extraction
    recCnt = 0       ;                   //monitor record count

    // Starting card reader
    Console::WriteLine("Realtime Card Monitoring begins now...");

    while (recCnt < 3)         //test to stop reading 3
    {
        strTemp =String::Concat( "8110" ,wudp->NumToStrHex(watchIndex, 2));            //build command frame wudp.NumToStrHex (watchIndex, 3) that the first watchIndex records, if it is the latest to take a record 0
        strCmd  = wudp->CreateBstrCommand(controllerSN, strTemp) ;                
        strFrame = wudp -> udp_comm(strCmd, ipAddr, ipPort);                        //send command and get back information
        if (( 0 != wudp ->ErrCode)||(strFrame->Length  ==0))    
        {
            Console::WriteLine("Error starting door monitor");
            //did not receive data
            //failure handling code ... (check ErrCode targeted analytical processing)
            break;
        }
        else
        {
            strSwipeDate = String::Concat(wudp->GetSwipeDateFromRunInfo(strFrame,  &cardId, &status));
            if (strSwipeDate->Length != 0 )
            {
                Console::WriteLine("(card_id,status_text,date) = ({0},{1},{2}",
                    cardId.ToString(),
                    status.ToString(),
                    strSwipeDate
                );
                
                if (watchIndex == 0)                //if received first record
                {
                    watchIndex = wudp->GetCardRecordCountFromRunInfo(strFrame);
                    watchIndex =  watchIndex + 1;   //Point (the total number of records +1), that is, the index bits stored in the next credit card
                }
                else
                {
                    watchIndex = watchIndex + 1 ; //point next record 
                }
                recCnt = recCnt + 1;                // record count
            }
        }
    }
    
    wudp = NULL;
}