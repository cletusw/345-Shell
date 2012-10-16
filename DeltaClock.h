#ifndef __os345_h__
#define __os345_h__
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
void print(DeltaClock* dc);
void insert(DeltaClock* dc, int tics, Semaphore* event);
void tic(DeltaClock* dc);
