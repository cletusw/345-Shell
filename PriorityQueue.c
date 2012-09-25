#include <stdlib.h>
#include <stdio.h>
#include "PriorityQueue.h"

PriorityQueue* newPriorityQueue(int length) {
	PriorityQueue* q = (PriorityQueue*) malloc(sizeof(PriorityQueue));
	q->length = length;
	q->first = (PriorityQueueItem*) malloc(sizeof(PriorityQueueItem) * length);
	return q;
}

void freePriorityQueue(PriorityQueue* q) {
	free(q->first);
	free(q);
}

void print(PriorityQueueItem q[], int length) {
	int i;
	
	for (i = 0; i < length; i++) {
		printf("{%d, %d} ", q[i].data, q[i].priority);
	}
	
	printf("\n");
}
