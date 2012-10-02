#define EMPTY_QUEUE -1

struct list_el {
	int data;
	int priority;
	struct list_el* next;
};

typedef struct list_el PriorityQueueItem;

typedef struct {
	PriorityQueueItem* head;
} PriorityQueue;

PriorityQueue* newPriorityQueue();
void deletePriorityQueue(PriorityQueue* q);
void print(PriorityQueue* q);
void enQ(PriorityQueue* q, int data, int priority);
int pop(PriorityQueue* q);
void deQ(PriorityQueue* q, int data);