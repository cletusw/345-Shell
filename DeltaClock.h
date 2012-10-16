#include "os345.h"
extern Semaphore* deltaClockMutex;

#ifdef TESTING
typedef int Semaphore;
#endif

struct _DeltaClockItem {
	int tics;
	Semaphore* event;
	struct _DeltaClockItem* next;
};

typedef struct _DeltaClockItem DeltaClockItem;

typedef struct {
	DeltaClockItem* head;
} DeltaClock;

DeltaClock* newDeltaClock();
void deleteDeltaClock(DeltaClock* dc);
void printClock(DeltaClock* dc);
void insert(DeltaClock* dc, int tics, Semaphore* event);
void tic(DeltaClock* dc);
