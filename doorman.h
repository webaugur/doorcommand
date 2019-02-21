#ifndef DOORMAN_H
#define DOORMAN_H
#include "doorcommand_dll.h"

/* I do declare */
void printValidCardCount(DoorCommand* command);
void printDateTime(DoorCommand* command);
void openDoors(DoorCommand* command, int door_numbers);
#endif