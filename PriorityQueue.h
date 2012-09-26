typedef struct {
	int data;
	int priority;
} PriorityQueueItem;

typedef struct {
	int length;
	PriorityQueueItem* array;
} PriorityQueue;

PriorityQueue* newPriorityQueue(int length);
void freePriorityQueue(PriorityQueue* q);
void print(PriorityQueue* q);
void enQ(PriorityQueue* q, int data, int priority);
void pop(PriorityQueue* q, int* data, int* priority);
void deQ(PriorityQueue* q, int data);