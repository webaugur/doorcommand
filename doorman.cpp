/* Doorman - Command Line Interface for Door Access Controllers
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
#include "doorman.h"
#include "doorcommand.h"
using namespace System;


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