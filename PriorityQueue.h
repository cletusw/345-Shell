typedef struct {
	int data;
	int priority;
} PriorityQueueItem;

typedef struct {
	int length;
	PriorityQueueItem* first;
} PriorityQueue;

PriorityQueue* newPriorityQueue(int length);
void freePriorityQueue(PriorityQueue* q);
void print(PriorityQueue* q);
void enQ(PriorityQueue* q, int data, int priority);
