// os345.c - OS Kernel
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the BYU CS345 projects.      **
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
#include <time.h>
#include <assert.h>

#include "os345.h"
#include "os345lc3.h"
#include "os345fat.h"
#include "DeltaClock.h"

// **********************************************************************
//	local prototypes
//
static void pollInterrupts(void);
static int scheduler(void);
static void recomputeTaskTimes(void);
static int dispatcher(void);

static void keyboard_isr(void);
static void timer_isr(void);

static int sysKillTask(int taskId);
static void initOS(void);

// **********************************************************************
// **********************************************************************
// global semaphores

Semaphore* semaphoreList;			// linked list of active semaphores

Semaphore* keyboard;				// keyboard semaphore
Semaphore* charReady;				// character has been entered
Semaphore* inBufferReady;			// input buffer ready semaphore

Semaphore* tics10sec;				// 10 second semaphore
Semaphore* tics1sec;				// 1 second semaphore
Semaphore* tics10thsec;				// 1/10 second semaphore

// **********************************************************************
// **********************************************************************
// global system variables

TCB tcb[MAX_TASKS];					// task control block
Semaphore* taskSems[MAX_TASKS];		// task semaphore
jmp_buf k_context;					// context of kernel stack
jmp_buf reset_context;				// context of kernel stack
volatile void* temp;				// temp pointer used in dispatcher

int scheduler_mode;					// scheduler mode
int superMode;						// system mode
int curTask;						// current task #
long swapCount;						// number of re-schedule cycles
char inChar;						// last entered character
int charFlag;						// 0 => buffered input
int inBufIndx;						// input pointer into input buffer
char inBuffer[INBUF_SIZE+1];		// character input buffer
Message messages[NUM_MESSAGES];		// process message buffers

int pollClock;						// current clock()
int lastPollClock;					// last pollClock
bool diskMounted;					// disk has been mounted

time_t oldTime1;					// old 1sec time
time_t oldTime10;					// old 10sec time
clock_t myClkTime;
clock_t myOldClkTime;
PriorityQueue* rq;					// ready priority queue
DeltaClock* dc;
Semaphore* deltaClockMutex;

// **********************************************************************
// **********************************************************************
// OS startup
//
// 1. Init OS
// 2. Define reset longjmp vector
// 3. Define global system semaphores
// 4. Create CLI task
// 5. Enter scheduling/idle loop
//
int main(int argc, char* argv[])
{
	// All the 'powerDown' invocations must occur in the 'main'
	// context in order to facilitate 'killTask'.  'killTask' must
	// free any stack memory associated with current known tasks.  As
	// such, the stack context must be one not associated with a task.
	// The proper method is to longjmp to the 'reset_context' that
	// restores the stack for 'main' and then invoke the 'powerDown'
	// sequence.

	// save context for restart (a system reset would return here...)
	int resetCode = setjmp(reset_context);
	superMode = TRUE;						// supervisor mode

	switch (resetCode)
	{
		case POWER_DOWN_QUIT:				// quit
			powerDown(0);
			printf("\nGoodbye!!");
			return 0;

		case POWER_DOWN_RESTART:			// restart
			powerDown(resetCode);
			printf("\nRestarting system...\n");

		case POWER_UP:						// startup
			break;

		default:
			printf("\nShutting down due to error %d", resetCode);
			powerDown(resetCode);
			return 0;
	}

	// output header message
	printf("%s", STARTUP_MSG);

	// initalize OS
	initOS();

	// create global/system semaphores here
	//?? vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	charReady = createSemaphore("charReady", BINARY, 0);
	inBufferReady = createSemaphore("inBufferReady", BINARY, 0);
	keyboard = createSemaphore("keyboard", BINARY, 1);
	tics10sec = createSemaphore("tics10sec", COUNTING, 0);
	tics1sec = createSemaphore("tics1sec", BINARY, 0);
	tics10thsec = createSemaphore("tics10thsec", BINARY, 0);

	//?? ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	// schedule CLI task
	createTask("myShell",			// task name
					P1_shellTask,	// task
					MED_PRIORITY,	// task priority
					argc,			// task arg count
					argv);			// task argument pointers

	// HERE WE GO................

	// Scheduling loop
	// 1. Check for asynchronous events (character inputs, timers, etc.)
	// 2. Choose a ready task to schedule
	// 3. Dispatch task
	// 4. Loop (forever!)

	while(1)									// scheduling loop
	{
		// check for character / timer interrupts
		pollInterrupts();

		// schedule highest priority ready task
		if ((curTask = scheduler()) < 0) continue;

		// dispatch curTask, quit OS if negative return
		if (dispatcher() < 0) break;
	}											// end of scheduling loop

	// exit os
	longjmp(reset_context, POWER_DOWN_QUIT);
	return 0;
} // end main



// **********************************************************************
// keyboard interrupt service routine
//
static void keyboard_isr()
{
	int i;
	
	// assert system mode
	assert("keyboard_isr Error" && superMode);

	semSignal(charReady);					// SIGNAL(charReady) (No Swap)
	if (charFlag == 0)
	{
		switch (inChar)
		{
			case '\r':
			case '\n':
			{
				inBufIndx = 0;				// EOL, signal line ready
				semSignal(inBufferReady);	// SIGNAL(inBufferReady)
				break;
			}
			
			// Backspace
			case 0x7f:
			{
				if (inBufIndx > 0) {
					inBufIndx--;
					inBuffer[inBufIndx] = 0;
					printf("\b \b");		// remove last character
				}
				break;
			}

			case 0x18:						// ^x
			{
				inBufIndx = 0;
				inBuffer[0] = 0;
				sigSignal(0, mySIGINT);		// interrupt task 0
				semSignal(inBufferReady);	// SEM_SIGNAL(inBufferReady)
				break;
			}
			
			case 0x17:						// ^w
			{
				sigSignal(-1, mySIGTSTP);	// stop all tasks
				break;
			}
			
			case 0x12:						// ^r
			{
				// Resume all tasks
				sigSignal(-1, mySIGCONT);
				
				// Clear mySIGSTOP and mySIGTSTP in all tasks
				for (i = 0; i < MAX_TASKS; i++) {
					tcb[i].signal &= ~mySIGTSTP;
					tcb[i].signal &= ~mySIGSTOP;
				}
				
				break;
			}
			
			case 0x1b:
			{
				if (GET_CHAR == 0x5b) {
					inBuffer[inBufIndx++] = inChar;
					inBuffer[inBufIndx++] = 0x5b;
					inBuffer[inBufIndx++] = GET_CHAR;
					inBuffer[inBufIndx] = 0;
					inBufIndx = 0;
					semSignal(inBufferReady);	// SEM_SIGNAL(inBufferReady)
				}
				break;
			}

			default:
			{
				inBuffer[inBufIndx++] = inChar;
				inBuffer[inBufIndx] = 0;
				printf("%c", inChar);		// echo character
			}
		}
	}
	else
	{
		// single character mode
		inBufIndx = 0;
		inBuffer[inBufIndx] = 0;
	}
	return;
} // end keyboard_isr


// **********************************************************************
// timer interrupt service routine
//
static void timer_isr()
{
	time_t currentTime;						// current time

	// assert system mode
	assert("timer_isr Error" && superMode);

	// capture current time
  	time(&currentTime);

	// ten second timer
	if ((currentTime - oldTime10) >= 10)
	{
		// signal 10 second
		semSignal(tics10sec);
		oldTime10 += 10;
	}

  	// one second timer
  	if ((currentTime - oldTime1) >= 1)
  	{
		// signal 1 second
  	   semSignal(tics1sec);
		oldTime1 += 1;
  	}

	// sample fine clock
	myClkTime = clock();
	if ((myClkTime - myOldClkTime) >= ONE_TENTH_SEC && deltaClockMutex)
	{
		myOldClkTime = myOldClkTime + ONE_TENTH_SEC;   // update old
		semSignal(tics10thsec);
		tic(dc);
	}

	return;
} // end timer_isr


// **********************************************************************
// **********************************************************************
// simulate asynchronous interrupts by polling events during idle loop
//
static void pollInterrupts(void)
{
	// check for task monopoly
	pollClock = clock();
	assert("Timeout" && ((pollClock - lastPollClock) < MAX_CYCLES));
	lastPollClock = pollClock;

	// check for keyboard interrupt
	if ((inChar = GET_CHAR) > 0)
	{
	  keyboard_isr();
	}

	// timer interrupt
	timer_isr();

	return;
} // end pollInterrupts



// **********************************************************************
// **********************************************************************
// scheduler
//
static int scheduler()
{
	int nextTask, firstTask;
	// ?? Design and implement a scheduler that will select the next highest
	// ?? priority ready task to pass to the system dispatcher.

	// ?? WARNING: You must NEVER call swapTask() from within this function
	// ?? or any function that it calls.  This is because swapping is
	// ?? handled entirely in the swapTask function, which, in turn, may
	// ?? call this function.  (ie. You would create an infinite loop.)

	// ?? Implement a round-robin, preemptive, prioritized scheduler.

	// ?? This code is simply a round-robin scheduler and is just to get
	// ?? you thinking about scheduling.  You must implement code to handle
	// ?? priorities, clean up dead tasks, and handle semaphores appropriately.

	// schedule next task
	firstTask = nextTask = pop(rq);

	// Fair-share scheduling
	if (scheduler_mode && nextTask >= 0) {
		while (tcb[nextTask].time < 1) {
			enQ(rq, nextTask, tcb[nextTask].priority);
			nextTask = pop(rq);

			if (nextTask == firstTask) {
				// No tasks left with nonzero time. Recompute.
				recomputeTaskTimes();
			}
		}

		tcb[nextTask].time--;
	}

	// mask sure nextTask is valid
	assert("Popped invalid task" && (nextTask == -1 || tcb[nextTask].name));

	if (tcb[nextTask].signal & mySIGSTOP) {
		enQ(rq, nextTask, tcb[nextTask].priority);
		return -1;
	}

	return nextTask;
} // end scheduler

void recomputeTaskTimes() {
	int numChildren[MAX_TASKS];
	int i, mostChildren = 0;
	int clocksPerGroup;

	// Count children of each task
	for (i = 0; i < MAX_TASKS; i++) {
		numChildren[i] = 0;
	}
	for (i = 0; i < MAX_TASKS; i++) {
		if (tcb[i].name && tcb[i].parent >= 0) {
			numChildren[tcb[i].parent]++;
		}
	}

	// Find task with most children
	for (i = 0; i < MAX_TASKS; i++) {
		if (numChildren[i] > numChildren[mostChildren]) {
			mostChildren = i;
		}
	}

	clocksPerGroup = numChildren[mostChildren] + 1;

	// Distribute CPU time
	for (i = 0; i < MAX_TASKS; i++) {
		if (tcb[i].name && numChildren[i] == 0) {
			// Leaf. Give clocksPerGroup/(numChildren[parent] + 1) time slots.
			tcb[i].time = clocksPerGroup/(numChildren[tcb[i].parent] + 1);

			// Set parent's time if not already set by sibling
			if (tcb[tcb[i].parent].time <= 0) {
				tcb[tcb[i].parent].time = tcb[i].time + clocksPerGroup % (numChildren[tcb[i].parent] + 1);
			}
		}
	}

	// Give remaining tasks 1 clock (shell, etc)
	for (i = 0; i < MAX_TASKS; i++) {
		if (tcb[i].name && tcb[i].time <= 0) {
			tcb[i].time = 1;
		}
	}
}



// **********************************************************************
// **********************************************************************
// dispatch curTask
//
static int dispatcher()
{
	int result;

	// schedule task
	switch(tcb[curTask].state)
	{
		case S_NEW:
		{
			// new task
			printf("\nNew Task[%d] %s", curTask, tcb[curTask].name);
			tcb[curTask].state = S_RUNNING;	// set task to run state

			// save kernel context for task SWAP's
			if (setjmp(k_context))
			{
				superMode = TRUE;					// supervisor mode
				break;								// context switch to next task
			}

			// move to new task stack (leave room for return value/address)
			temp = (int*)tcb[curTask].stack + (STACK_SIZE-8);
			SET_STACK(temp);
			superMode = FALSE;						// user mode

			// begin execution of new task, pass argc, argv
			result = (*tcb[curTask].task)(tcb[curTask].argc, tcb[curTask].argv);

			// task has completed
			if (result) printf("\nTask[%d] returned %d", curTask, result);
			else printf("\nTask[%d] returned %d", curTask, result);
			tcb[curTask].state = S_EXIT;			// set task to exit state

			// return to kernal mode
			longjmp(k_context, 1);					// return to kernel
		}

		case S_READY:
		{
			tcb[curTask].state = S_RUNNING;			// set task to run
		}

		case S_RUNNING:
		{
			if (setjmp(k_context))
			{
				// SWAP executed in task
				superMode = TRUE;					// supervisor mode
				break;								// return from task
			}
			if (tcb[curTask].signal)
			{
				if (tcb[curTask].signal & mySIGINT)
				{
					tcb[curTask].signal &= ~mySIGINT;
					(*tcb[curTask].sigIntHandler)();
				}

				if (tcb[curTask].signal & mySIGTERM)
				{
					tcb[curTask].signal &= ~mySIGTERM;
					(*tcb[curTask].sigTermHandler)();
					enQ(rq, curTask, tcb[curTask].priority);	// ??
					return 0;						// do not schedule task
				}

				if (tcb[curTask].signal & mySIGTSTP)
				{
					tcb[curTask].signal &= ~mySIGTSTP;
					(*tcb[curTask].sigTstpHandler)();
					enQ(rq, curTask, tcb[curTask].priority);
					return 0;						// do not schedule task
				}

				if (tcb[curTask].signal & mySIGCONT)
				{
					tcb[curTask].signal &= ~mySIGCONT;
					(*tcb[curTask].sigContHandler)();
				}
			}

			longjmp(tcb[curTask].context, 3); 		// restore task context
		}

		case S_BLOCKED:
		{
			// ?? Could check here to unblock task
			break;
		}

		case S_EXIT:
		{
			if (curTask == 0) return -1;			// if CLI, then quit scheduler
			// release resources and kill task
			sysKillTask(curTask);					// kill current task
			break;
		}

		default:
		{
			printf("Unknown Task[%d] State", curTask);
			longjmp(reset_context, POWER_DOWN_ERROR);
		}
	}
	return 0;
} // end dispatcher



// **********************************************************************
// **********************************************************************
// Do a context switch to next task.

// 1. If scheduling task, return (setjmp returns non-zero value)
// 2. Else, save current task context (setjmp returns zero value)
// 3. Set current task state to READY
// 4. Enter kernel mode (longjmp to k_context)

void swapTask()
{
	assert("SWAP Error" && !superMode);		// assert user mode

	// increment swap cycle counter
	swapCount++;

	// either save current task context or schedule task (return)
	if (setjmp(tcb[curTask].context))
	{
		superMode = FALSE;					// user mode
		return;
	}

	// context switch - move task state to ready
	if (tcb[curTask].state == S_RUNNING) {
		tcb[curTask].state = S_READY;
		enQ(rq, curTask, tcb[curTask].priority);
	}

	// move to kernel mode (reschedule)
	longjmp(k_context, 2);
} // end swapTask



// **********************************************************************
// **********************************************************************
// system utility functions
// **********************************************************************
// **********************************************************************

// **********************************************************************
// **********************************************************************
// initialize operating system
static void initOS()
{
	int i;

	// make any system adjustments (for unblocking keyboard inputs)
	INIT_OS

	// reset system variables
	curTask = 0;						// current task #
	swapCount = 0;						// number of scheduler cycles
	scheduler_mode = 0;					// default scheduler
	inChar = 0;							// last entered character
	charFlag = 0;						// 0 => buffered input
	inBufIndx = 0;						// input pointer into input buffer
	semaphoreList = 0;					// linked list of active semaphores
	diskMounted = 0;					// disk has been mounted

	// malloc ready queue
	rq = newPriorityQueue(MAX_TASKS);

	// Set up delta clock
	dc = newDeltaClock();
	deltaClockMutex = createSemaphore("deltaClockMutex", BINARY, 1);

	// capture current time
	lastPollClock = clock();			// last pollClock
   time(&oldTime1);
   time(&oldTime10);

	// init system tcb's
	for (i=0; i<MAX_TASKS; i++)
	{
		tcb[i].name = NULL;				// tcb
		taskSems[i] = NULL;				// task semaphore
	}

	// initalize message buffers
	for (i=0; i<NUM_MESSAGES; i++)
	{
		messages[i].to = -1;
	}

	// init tcb
	for (i=0; i<MAX_TASKS; i++)
	{
		tcb[i].name = NULL;
	}

	// initialize lc-3 memory
	initLC3Memory(LC3_MEM_FRAME, 0xF800>>6);

	// ?? initialize all execution queues

	return;
} // end initOS



// **********************************************************************
// **********************************************************************
// Causes the system to shut down. Use this for critical errors
void powerDown(int code)
{
	int i;
	printf("\nPowerDown Code %d", code);

	// release all system resources.
	printf("\nRecovering Task Resources...");

	// kill all tasks
	for (i = MAX_TASKS-1; i >= 0; i--)
		if(tcb[i].name) sysKillTask(i);

	// delete all semaphores
	while (semaphoreList)
		deleteSemaphore(&semaphoreList);

	// free ready queue
	deletePriorityQueue(rq);

	// release any other system resources
	// deltaclock (project 3)
	deleteDeltaClock(dc);

	RESTORE_OS
	return;
} // end powerDown



// **********************************************************************
// **********************************************************************
//	Signal handlers
//
int sigAction(void (*sigHandler)(void), int sig)
{
	switch (sig)
	{
		case mySIGINT:
		{
			tcb[curTask].sigIntHandler = sigHandler;		// mySIGINT handler
			return 0;
		}
		case mySIGTERM:
		{
			tcb[curTask].sigTermHandler = sigHandler;		// mySIGTERM handler
			return 0;
		}
		case mySIGTSTP:
		{
			tcb[curTask].sigTstpHandler = sigHandler;		// mySIGTSTP handler
			return 0;
		}
		case mySIGCONT:
		{
			tcb[curTask].sigContHandler = sigHandler;		// mySIGCONT handler
			return 0;
		}
	}
	return 1;
}


// **********************************************************************
//	sigSignal - send signal to task(s)
//
//	taskId = task (-1 = all tasks)
//	sig = signal
//
int sigSignal(int taskId, int sig)
{
	// check for task
	if ((taskId >= 0) && tcb[taskId].name)
	{
		tcb[taskId].signal |= sig;
		return 0;
	}
	else if (taskId == -1)
	{
		for (taskId=0; taskId<MAX_TASKS; taskId++)
		{
			sigSignal(taskId, sig);
		}
		return 0;
	}
	// error
	return 1;
}


// **********************************************************************
// **********************************************************************
//	Default signal handlers
//
void defaultSigIntHandler(void)			// task mySIGINT handler
{
	printf("\ndefaultSigIntHandler");
	return;
}

void defaultSigTermHandler(void)		// task mySIGTERM handler
{
	printf("\ndefaultSigTermHandler");
	return;
}

void defaultSigTstpHandler(void)		// task mySIGTSTP handler
{
	printf("\ndefaultSigTstpHandler");
	return;
}

void defaultSigContHandler(void)		// task mySIGCONT handler
{
	printf("\ndefaultSigContHandler");
	return;
}


// **********************************************************************
// **********************************************************************
// create task
int createTask(char* name,						// task name
					int (*task)(int, char**),	// task address
					int priority,				// task priority
					int argc,					// task argument count
					char* argv[])				// task argument pointers
{
	int tid;
	int i;
	char** newArgv;

	// find an open tcb entry slot
	for (tid = 0; tid < MAX_TASKS; tid++)
	{
		if (tcb[tid].name == 0)
		{
			char buf[8];

			// create task semaphore
			if (taskSems[tid]) deleteSemaphore(&taskSems[tid]);
			sprintf(buf, "task%d", tid);
			taskSems[tid] = createSemaphore(buf, 0, 0);
			taskSems[tid]->taskNum = 0;	// assign to shell

			// copy task name
			tcb[tid].name = (char*)malloc(strlen(name)+1);
			strcpy(tcb[tid].name, name);

			// set task address and other parameters
			tcb[tid].task = task;			// task address
			tcb[tid].state = S_NEW;			// NEW task state
			tcb[tid].priority = priority;	// task priority
			tcb[tid].parent = curTask;		// parent
			tcb[tid].argc = argc;			// argument count

			// Allocate space for argv
			newArgv = (char**) malloc(sizeof(char*) * argc);
			
			// Allocate space for parameters
			for (i = 0; i < argc; i++) {
				newArgv[i] = (char*) malloc(sizeof(char) * (strlen(argv[i]) + 1));
				strcpy(newArgv[i], argv[i]);
			}
			
			tcb[tid].argv = newArgv;			// argument pointers

			tcb[tid].event = 0;				// suspend semaphore
			tcb[tid].RPT = 0;					// root page table (project 5)
			tcb[tid].cdir = CDIR;			// inherit parent cDir (project 6)

			// signals
			tcb[tid].signal = 0;
			if (tid)
			{
				// inherit parent signal handlers
				tcb[tid].sigIntHandler = tcb[curTask].sigIntHandler;			// mySIGINT handler
				tcb[tid].sigTermHandler = tcb[curTask].sigTermHandler;			// mySIGTERM handler
				tcb[tid].sigTstpHandler = tcb[curTask].sigTstpHandler;			// mySIGTSTP handler
				tcb[tid].sigContHandler = tcb[curTask].sigContHandler;			// mySIGCONT handler
			}
			else
			{
				// otherwise use defaults
				tcb[tid].sigIntHandler = defaultSigIntHandler;			// task mySIGINT handler
				tcb[tid].sigTermHandler = defaultSigTermHandler;			// task mySIGTERM handler
				tcb[tid].sigTstpHandler = defaultSigTstpHandler;			// task mySIGTSTP handler
				tcb[tid].sigContHandler = defaultSigContHandler;			// task mySIGCONT handler
			}

			// Each task must have its own stack and stack pointer.
			tcb[tid].stack = malloc(STACK_SIZE * sizeof(int));

			// Start off with zero clock time
			tcb[tid].time = 0;

			// Insert task into "ready" queue
			enQ(rq, tid, priority);

			if (tid) swapTask();				// do context switch (if not cli)
			return tid;							// return tcb index (curTask)
		}
	}
	// tcb full!
	return -1;
} // end createTask



// **********************************************************************
// **********************************************************************
// kill task
//
//	taskId == -1 => kill all non-shell tasks
//
static void exitTask(int taskId);
int killTask(int taskId)
{
	int tid, i;
	char** argv;
	assert("killTask Error" && (taskId == -1 || tcb[taskId].name));

	if (taskId != 0)			// don't terminate shell
	{
		if (taskId < 0)		// kill all tasks
		{
			for (tid = 0; tid < MAX_TASKS; tid++)
			{
				if (tcb[tid].name) {
					exitTask(tid);
					
					// Free argv
					argv = tcb[taskId].argv;
					for (i = 0; i < tcb[taskId].argc && argv[i]; i++) {
						free(argv[i]);
					}
					free(argv);
				}
			}
		}
		else
		{
			// terminate individual task
			exitTask(taskId);	// kill individual task
			
			// Free argv
			argv = tcb[taskId].argv;
			for (i = 0; i < tcb[taskId].argc && argv[i]; i++) {
				free(argv[i]);
			}
			free(argv);
		}
	}
	if (!superMode) SWAP;
	return 0;
} // end killTask

static void exitTask(int taskId)
{
	assert("exitTaskError" && tcb[taskId].name);

	// 1. find task in system queue
	// 2. if blocked, unblock (handle semaphore)
	// 3. set state to exit
	
	tcb[taskId].state = S_EXIT;			// EXIT task state

	return;
} // end exitTask



// **********************************************************************
// system kill task
//
static int sysKillTask(int taskId)
{
	Semaphore* sem = semaphoreList;
	Semaphore** semLink = &semaphoreList;

	// assert that you are not pulling the rug out from under yourself!
	assert("sysKillTask Error" && tcb[taskId].name && superMode);
	printf("\nKill Task %s", tcb[taskId].name);

	// signal task terminated
	semSignal(taskSems[taskId]);

	// look for any semaphores created by this task
	while(sem = *semLink)
	{
		if(sem->taskNum == taskId)
		{
			// semaphore found, delete from list, release memory
			deleteSemaphore(semLink);
		}
		else
		{
			// Delete task from blocked queues
			deQ(sem->blockedTasks, taskId);
			
			// move to next semaphore
			semLink = (Semaphore**)&sem->semLink;
		}
	}

	// delete task from ready queue
	deQ(rq, taskId);

	tcb[taskId].name = 0;			// release tcb slot
	return 0;
} // end killTask



// **********************************************************************
// **********************************************************************
// signal semaphore
//
//	if task blocked by semaphore, then clear semaphore and wakeup task
//	else signal semaphore
//
void semSignal(Semaphore* s)
{
	int taskId;
	// assert there is a semaphore and it is a legal type
	assert("semSignal Error" && s && ((s->type == 0) || (s->type == 1)));

	// check semaphore type
	if (s->type == 0)
	{
		// binary semaphore
		// look through tasks for one suspended on this semaphore

		if ((taskId = pop(s->blockedTasks)) >= 0) {
			assert("Popped wrong task from semaphore blocked queue" && tcb[taskId].event == s);

			s->state = 0;				// clear semaphore
			tcb[taskId].event = 0;			// clear event pointer
			tcb[taskId].state = S_READY;	// unblock task

			// move task from blocked to ready queue
			enQ(rq, taskId, tcb[taskId].priority);

			if (!superMode) swapTask();
			return;
		}
		// nothing waiting on semaphore, go ahead and just signal
		s->state = 1;						// nothing waiting, signal
		if (!superMode) swapTask();
		return;
	}
	else
	{
		// counting semaphore
		if (++s->state > 0) return;

		taskId = pop(s->blockedTasks);
		tcb[taskId].event = 0;			// clear event pointer
		tcb[taskId].state = S_READY;
		enQ(rq, taskId, tcb[taskId].priority);
	}
} // end semSignal



// **********************************************************************
// **********************************************************************
// wait on semaphore
//
//	if semaphore is signaled, return immediately
//	else block task
//
int semWait(Semaphore* s)
{
	assert("semWait Error" && s);												// assert semaphore
	assert("semWait Error" && ((s->type == 0) || (s->type == 1)));	// assert legal type
	assert("semWait Error" && !superMode);								// assert user mode

	// check semaphore type
	if (s->type == 0)
	{
		// binary semaphore
		// if state is zero, then block task

		if (s->state == 0)
		{
			tcb[curTask].event = s;		// block task
			tcb[curTask].state = S_BLOCKED;

			// move task from ready queue to blocked queue
			enQ(s->blockedTasks, curTask, tcb[curTask].priority);

			swapTask();						// reschedule the tasks
			return 1;
		}
		// state is non-zero (semaphore already signaled)
		s->state = 0;						// reset state, and don't block
		return 0;
	}
	else
	{
		// counting semaphore
		s->state--;					// consume
		if (s->state >= 0) return;	// if available, return

		// resource not available, block task
		tcb[curTask].event = s;		// block task
		tcb[curTask].state = S_BLOCKED;	// change task state to blocked
		deQ(rq, curTask);				// move from ready  to blocked queue
		enQ(s->blockedTasks, curTask, tcb[curTask].priority);
		swapTask();				// reschedule the tasks
		return;					// returning from blocked state
	}
} // end semWait



// **********************************************************************
// **********************************************************************
// try to wait on semaphore
//
//	if semaphore is signaled, return 1
//	else return 0
//
int semTryLock(Semaphore* s)
{
	assert("semTryLock Error" && s);												// assert semaphore
	assert("semTryLock Error" && ((s->type == 0) || (s->type == 1)));	// assert legal type
	assert("semTryLock Error" && !superMode);									// assert user mode

	// check semaphore type
	if (s->type == 0)
	{
		// binary semaphore
		// if state is zero, then block task

temp:	// ?? temporary label
		if (s->state == 0)
		{
			return 0;
		}
		// state is non-zero (semaphore already signaled)
		s->state = 0;						// reset state, and don't block
		return 1;
	}
	else
	{
		// counting semaphore
		// ?? implement counting semaphore

		goto temp;
	}
} // end semTryLock


// **********************************************************************
// **********************************************************************
// Create a new semaphore.
// Use heap memory (malloc) and link into semaphore list (Semaphores)
// 	name = semaphore name
//		type = binary (0), counting (1)
//		state = initial semaphore state
// Note: memory must be released when the OS exits.
//
Semaphore* createSemaphore(char* name, int type, int state)
{
	Semaphore* sem = semaphoreList;
	Semaphore** semLink = &semaphoreList;

	// assert semaphore is binary or counting
	assert("createSemaphore Error" && ((type == 0) || (type == 1)));	// assert type is validate

	// look for duplicate name
	while (sem)
	{
		if (!strcmp(sem->name, name))
		{
			printf("\nSemaphore %s already defined", sem->name);

			// ?? What should be done about duplicate semaphores ??
			// semaphore found - change to new state
			sem->type = type;					// 0=binary, 1=counting
			sem->state = state;				// initial semaphore state
			sem->taskNum = curTask;			// set parent task #
			return sem;
		}
		// move to next semaphore
		semLink = (Semaphore**)&sem->semLink;
		sem = (Semaphore*)sem->semLink;
	}

	// allocate memory for new semaphore
	sem = (Semaphore*)malloc(sizeof(Semaphore));

	// set semaphore values
	sem->name = (char*)malloc(strlen(name)+1);
	strcpy(sem->name, name);				// semaphore name
	sem->type = type;							// 0=binary, 1=counting
	sem->state = state;						// initial semaphore state
	sem->taskNum = curTask;					// set parent task #
	sem->blockedTasks = newPriorityQueue(MAX_TASKS);

	// prepend to semaphore list
	sem->semLink = (struct semaphore*)semaphoreList;
	semaphoreList = sem;						// link into semaphore list
	return sem;									// return semaphore pointer
} // end createSemaphore



// **********************************************************************
// **********************************************************************
// Delete semaphore and free its resources
//
bool deleteSemaphore(Semaphore** semaphore)
{
	Semaphore* sem = semaphoreList;
	Semaphore** semLink = &semaphoreList;

	// assert there is a semaphore
	assert("deleteSemaphore Error" && *semaphore);

	// look for semaphore
	while(sem)
	{
		if (sem == *semaphore)
		{
			// semaphore found, delete from list, release memory
			*semLink = (Semaphore*)sem->semLink;

			// free the name array before freeing semaphore
			printf("\ndeleteSemaphore(%s)", sem->name);

			// ?? free all semaphore memory
			free(sem->name);
			deletePriorityQueue(sem->blockedTasks);
			free(sem);

			return TRUE;
		}
		// move to next semaphore
		semLink = (Semaphore**)&sem->semLink;
		sem = (Semaphore*)sem->semLink;
	}

	// could not delete
	return FALSE;
} // end deleteSemaphore



// **********************************************************************
// **********************************************************************
// post a message to the message buffers
//
int postMessage(int from, int to, char* msg)
{
	int i;
	// insert message in open slot
	for (i=0; i<NUM_MESSAGES; i++)
	{
		if (messages[i].to == -1)
		{
			//printf("\n(%d) Send from %d to %d: (%s)", i, from, to, msg);
			messages[i].from = from;
			messages[i].to = to;
			messages[i].msg = malloc(strlen(msg)+1);
			strcpy(messages[i].msg, msg);
			return 1;
		}
	}
	printf("\n  **Message buffer full!  Message (%d,%d: %s) not sent.", from, to, msg);
	return 0;
} // end postMessage



// **********************************************************************
// **********************************************************************
// retrieve a message from the message buffers
//
int getMessage(int from, int to, Message* msg)
{
	int i;
	for (i=0; i<NUM_MESSAGES; i++)
	{
		if ((messages[i].to == to) && ((messages[i].from == from) || (from == -1)))
		{
			// get copy of message
			msg->from = messages[i].from;
			msg->to = messages[i].to;
			msg->msg = messages[i].msg;

			// roll list down
			for (; i<NUM_MESSAGES-1; i++)
			{
				messages[i] = messages[i+1];
				if (messages[i].to < 0) break;
			}
			messages[i].to = -1;
			return 0;
		}
	}
	printf("\n  **No message from %d to %d", from, to);
	return 1;
} // end getMessage



// **********************************************************************
// **********************************************************************
// read current time
//
char* myTime(char* svtime)
{
	time_t cTime;						// current time

	time(&cTime);						// read current time
	strcpy(svtime, asctime(localtime(&cTime)));
	svtime[strlen(svtime)-1] = 0;		// eliminate nl at end
	return svtime;
} // end myTime
