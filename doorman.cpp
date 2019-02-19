// This is the main project file for VC++ application project 
// generated using an Application Wizard.

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>
#using <mscorlib.dll>

using namespace System;

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "tchar.h"
#include "getopt.h"



int main (int argc, char **argv)
{
  int aflag = 0;
  int bflag = 0;
  char *cvalue = NULL;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "abc:")) != -1)
    switch (c)
      {
      case 'a':
        aflag = 1;
        break;
      case 'b':
        bflag = 1;
        break;
      case 'c':
        cvalue = optarg;
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

  printf ("aflag = %d, bflag = %d, cvalue = %s\n",
          aflag, bflag, cvalue);

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);














    // TODO: Please replace the sample code below with your own.
    // Console::WriteLine(S"Hello World");
    // Console::WriteLine("Current code page: {0}\n", 
    //                    Console.OutputEncoding.CodePage);

	WComm_UDP::IWCOMM_OPERATE		*wudp;
    wudp = new WComm_UDP::WComm_Operate();

    String* strTemp;
    String* strFrame;	  // Return Data
    String* strCmd;       // To send the command frame
    __int64 controllerSN; // Controller Series No.
    int ipPort;       // Controller TCP Port
    String* ipAddr;		  // IP Address
			      
    // Variable card records
    __int64  cardId;		//Card Number 
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
    strFrame = wudp -> udp_comm(strCmd, ipAddr, ipPort);	//send command and get back information
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
    strFrame = wudp -> udp_comm(strCmd, ipAddr, ipPort);	                    //Send command, and get back information
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
    //read to run state is the key to achieving control instructions. Conducting surveillance, the first record of the index to read the 					//latest bit of records. Read the 					//latest record, while the number of records can get to the credit card.
    //Then you can use to read the card plus 1 to fill the index to read the latest position on the record, to read the running to get the 					//next record.
    //If you read into a new credit card records, it can be add index wth 1 bit, otherwise the bit to keep the index unchanged. This allows 					//real-time monitoring data.
    //face communication is not the treatment, serial communication can take this time out as errors of 400-500 milliseconds, can try again, 					//and then not receive data, can be 					//considered between device and PC, can not communicate.
    __int64 watchIndex;
    long recCnt;

    watchIndex = 0   ;                   //default from 0, indicating a recent record of the first extraction
    recCnt = 0       ;                   //monitor record count

    // Starting card reader
    Console::WriteLine("Realtime Card Monitoring begins now...");

	while (recCnt < 3)         //test to stop reading 3
	{
        strTemp =String::Concat( "8110" ,wudp->NumToStrHex(watchIndex, 2));			//build command frame wudp.NumToStrHex (watchIndex, 3) that the first watchIndex records, if it is the latest to take a record 0
        strCmd  = wudp->CreateBstrCommand(controllerSN, strTemp) ;                
        strFrame = wudp -> udp_comm(strCmd, ipAddr, ipPort);	                    //send command and get back information
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
	return 0;
}