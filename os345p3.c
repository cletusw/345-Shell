// os345p3.c - Jurassic Park
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the CS345 projects.          **
// ** It comes "as is" and "unwarranted."  As such, when you use part   **
// ** or all of the code, it becomes "yours" and you are responsible to **
// ** understand any algorithm or method presented.  Likewise, any      **
// ** errors or problems become your responsibility to fix.             **
// **                                                                   **
// ** NOTES:                                                            **
// ** -Comments beginning with "// ??" may require some implementation. **
// ** -Tab stops are set at every 3 spaces.                             **
// ** -The function API's in "OS345.h" should not be altered.           **
// **                                                                   **
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// ***********************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <time.h>
#include <assert.h>
#include "os345.h"
#include "os345park.h"

#include "DeltaClock.h"

// ***********************************************************************
// project 3 variables

// Jurassic Park
extern JPARK myPark;
extern Semaphore* parkMutex;						// protect park access
extern Semaphore* fillSeat[NUM_CARS];			// (signal) seat ready to fill
extern Semaphore* seatFilled[NUM_CARS];		// (wait) passenger seated
extern Semaphore* rideOver[NUM_CARS];			// (signal) ride over

extern DeltaClock* dc;
extern Semaphore* deltaClockMutex;

Semaphore* getPassenger;
Semaphore* seatTaken;
Semaphore* rideDone[MAX_TASKS];
Semaphore* timeEvent[MAX_TASKS];
Semaphore* passengerSeated;
int curVisitor;
Semaphore* visitorMutex;
Semaphore* spotInPark;

#define MAX_ENTRANCE_TIME 10			// in seconds


// ***********************************************************************
// project 3 functions and tasks
int P3_carTask(int argc, char* argv[]);
int P3_visitorTask(int argc, char* argv[]);


// ***********************************************************************
// ***********************************************************************
// project3 command
int P3_project3(int argc, char* argv[])
{
	int i;
	char buf[32];
	char carId;
	char* newArgv[2];

	// start park
	sprintf(buf, "jurassicPark");
	newArgv[0] = buf;
	createTask( buf,				// task name
		jurassicTask,				// task
		MED_PRIORITY,				// task priority
		1,								// task count
		newArgv);					// task argument

	getPassenger = createSemaphore("getPassenger", BINARY, 0);
	seatTaken = createSemaphore("seatTaken", BINARY, 0);
	passengerSeated = createSemaphore("passengerSeated", BINARY, 1);
	visitorMutex = createSemaphore("visitorMutex", BINARY, 1);
	spotInPark = createSemaphore("spotInPark", COUNTING, MAX_IN_PARK);

	// wait for park to get initialized...
	while (!parkMutex) SWAP;
	printf("\nStart Jurassic Park...");

	//?? create car, driver, and visitor tasks here
	sprintf(buf, "carTask");			SWAP;
	carId = 0;			SWAP;
	newArgv[0] = buf;			SWAP;
	newArgv[1] = &carId;			SWAP;
	createTask(buf, P3_carTask, MED_PRIORITY, 2, newArgv);			SWAP;
	carId = 1;			SWAP;
	createTask(buf, P3_carTask, MED_PRIORITY, 2, newArgv);			SWAP;
	carId = 2;			SWAP;
	createTask(buf, P3_carTask, MED_PRIORITY, 2, newArgv);			SWAP;			SWAP;
	carId = 3;			SWAP;
	createTask(buf, P3_carTask, MED_PRIORITY, 2, newArgv);			SWAP;

	sprintf(buf, "visitorTask");			SWAP;
	newArgv[0] = buf;			SWAP;
	newArgv[1] = &i;			SWAP;
	for (i = 0; i < 22; i++) {
		createTask(buf, P3_visitorTask, MED_PRIORITY, 2, newArgv);			SWAP;
	}

	return 0;
} // end project3



// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char* argv[])
{
	// Display the current delta clock contents
	printf("\nDelta Clock\n");
	printClock(dc);

	return 0;
} // end CL3_dc


// ***********************************************************************
// ***********************************************************************
// Car Task
int P3_carTask(int argc, char* argv[]) {
	int i;			SWAP;
	Semaphore* carRideDone[NUM_SEATS];			SWAP;
	int carId = argv[1][0];				SWAP;

	while (1) {
		for (i = 0; i < NUM_SEATS; i++) {
			semWait(fillSeat[carId]);			SWAP;

			semSignal(getPassenger);			SWAP;	// signal for visitor
			semWait(seatTaken);			SWAP;	// wait for visitor to reply

			// save passenger ride over semaphore
			carRideDone[i] = rideDone[curVisitor];

			semSignal(passengerSeated);			SWAP;	// signal visitor in seat

			semSignal(seatFilled[carId]);			SWAP;
			semWait(parkMutex);			SWAP;
			myPark.numInCarLine--;			SWAP;
			myPark.numInCars++;			SWAP;
			semSignal(parkMutex);				SWAP;
		}

		semWait(rideOver[carId]);			SWAP;

		// Release passengers
		for (i = 0; i < NUM_SEATS; i++) {
			semWait(parkMutex);			SWAP;
			myPark.numInCars--;			SWAP;
			myPark.numInGiftLine++;				SWAP;
			semSignal(parkMutex);				SWAP;
			semSignal(carRideDone[i]);			SWAP;
		}
	}
}


// ***********************************************************************
// ***********************************************************************
// Visitor Task
int P3_visitorTask(int argc, char* argv[]) {
	int visitorId = argv[1][0];				SWAP;

	// Set up visitor-specific semaphores
	char buf[32];			SWAP;
	sprintf(buf, "timeEvent%d", visitorId);			SWAP;
	timeEvent[visitorId] = createSemaphore(buf, BINARY, 0);			SWAP;
	sprintf(buf, "rideDone%d", visitorId);			SWAP;
	rideDone[visitorId] = createSemaphore(buf, BINARY, 0);			SWAP;

	semWait(parkMutex);			SWAP;
	myPark.numOutsidePark++;			SWAP;
	semSignal(parkMutex);			SWAP;

	// Wait random time before attempting to enter
	int waitTime = rand() % (MAX_ENTRANCE_TIME * 10) + 1;			SWAP;
	insert(dc, waitTime, timeEvent[visitorId]);			SWAP;
	semWait(timeEvent[visitorId]);			SWAP;

	// Wait for a spot in the park
	semWait(spotInPark);			SWAP;

	// Enter park and ride the tour car
	semWait(parkMutex);				SWAP;
	myPark.numOutsidePark--;			SWAP;
	myPark.numInPark++;				SWAP;
	myPark.numInCarLine++;				SWAP;
	semSignal(parkMutex);				SWAP;

	semWait(getPassenger);				SWAP;

	semWait(visitorMutex);			SWAP;
	curVisitor = visitorId;			SWAP;
	semSignal(seatTaken);				SWAP;
	semWait(passengerSeated);			SWAP;
	semSignal(visitorMutex);			SWAP;

	semWait(rideDone[visitorId]);			SWAP;
}


/*
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char* argv[])
{
	printf("\nDelta Clock");
	// ?? Implement a routine to display the current delta clock contents
	//printf("\nTo Be Implemented!");
	int i;
	for (i=0; i<numDeltaClock; i++)
	{
		printf("\n%4d%4d  %-20s", i, deltaClock[i].time, deltaClock[i].sem->name);
	}
	return 0;
} // end CL3_dc


// ***********************************************************************
// display all pending events in the delta clock list
void printDeltaClock(void)
{
	int i;
	for (i=0; i<numDeltaClock; i++)
	{
		printf("\n%4d%4d  %-20s", i, deltaClock[i].time, deltaClock[i].sem->name);
	}
	return;
}


// ***********************************************************************
// test delta clock
int P3_tdc(int argc, char* argv[])
{
	createTask( "DC Test",			// task name
		dcMonitorTask,		// task
		10,					// task priority
		argc,					// task arguments
		argv);

	timeTaskID = createTask( "Time",		// task name
		timeTask,	// task
		10,			// task priority
		argc,			// task arguments
		argv);
	return 0;
} // end P3_tdc



// ***********************************************************************
// monitor the delta clock task
int dcMonitorTask(int argc, char* argv[])
{
	int i, flg;
	char buf[32];
	// create some test times for event[0-9]
	int ttime[10] = {
		90, 300, 50, 170, 340, 300, 50, 300, 40, 110	};

	for (i=0; i<10; i++)
	{
		sprintf(buf, "event[%d]", i);
		event[i] = createSemaphore(buf, BINARY, 0);
		insertDeltaClock(ttime[i], event[i]);
	}
	printDeltaClock();

	while (numDeltaClock > 0)
	{
		SEM_WAIT(dcChange)
		flg = 0;
		for (i=0; i<10; i++)
		{
			if (event[i]->state ==1)			{
					printf("\n  event[%d] signaled", i);
					event[i]->state = 0;
					flg = 1;
				}
		}
		if (flg) printDeltaClock();
	}
	printf("\nNo more events in Delta Clock");

	// kill dcMonitorTask
	tcb[timeTaskID].state = S_EXIT;
	return 0;
} // end dcMonitorTask


extern Semaphore* tics1sec;

// ********************************************************************************************
// display time every tics1sec
int timeTask(int argc, char* argv[])
{
	char svtime[64];						// ascii current time
	while (1)
	{
		SEM_WAIT(tics1sec)
		printf("\nTime = %s", myTime(svtime));
	}
	return 0;
} // end timeTask
*/

