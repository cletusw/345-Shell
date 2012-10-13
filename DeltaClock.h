struct _DeltaClockItem {
	int tics;
	void (*callback)(void);
	struct _DeltaClockItem* next;
};

typedef struct _DeltaClockItem DeltaClockItem;

typedef struct {
	DeltaClockItem* head;
} DeltaClock;

DeltaClock* newDeltaClock();
void deleteDeltaClock(DeltaClock* dc);
void print(DeltaClock* dc);
void insert(DeltaClock* dc, int tics, void (*callback)(void));