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
#include "doorcommand_dll.h"
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

   /* Humans are constantly arguing with us programs */
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
