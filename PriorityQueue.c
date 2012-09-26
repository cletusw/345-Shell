#include <stdlib.h>
#include <stdio.h>
#include "PriorityQueue.h"

PriorityQueue* newPriorityQueue(int length) {
	PriorityQueue* q = (PriorityQueue*) malloc(sizeof(PriorityQueue));
	q->length = 0;
	q->first = (PriorityQueueItem*) malloc(sizeof(PriorityQueueItem) * length);
	return q;
}

void freePriorityQueue(PriorityQueue* q) {
	free(q->first);
	free(q);
}

void print(PriorityQueue* q) {
	int i;
	int length = q->length;
	PriorityQueueItem* qArray = q->first;
	
	for (i = 0; i < length; i++) {
		printf("{%d, %d} ", qArray[i].data, qArray[i].priority);
	}
	
	printf("\n");
}

void enQ(PriorityQueue* q, int data, int priority) {
	
}
