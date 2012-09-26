#include <stdlib.h>
#include <stdio.h>
#include "PriorityQueue.h"

#define parent(k) ((k-1)/2)

// Helper declarations
void swim(PriorityQueue* q, int itemIndex);

// Public API
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
	int newItemIndex = q->length;
	PriorityQueueItem* item = &(q->first[newItemIndex]);
	item->data = data;
	item->priority = priority;
	q->length++;
	swim(q, newItemIndex);
}

PriorityQueueItem* pop(PriorityQueue* q) {
	q->length--;
}

// Helper definitions
void swim(PriorityQueue* q, int itemIndex) {
	PriorityQueueItem* qArray = q->first;
	PriorityQueueItem temp;

	while (itemIndex > 0 && qArray[parent(itemIndex)].priority < qArray[itemIndex].priority) {
		// Swap item and parent
		temp = qArray[parent(itemIndex)];
		qArray[parent(itemIndex)] = qArray[itemIndex];
		qArray[itemIndex] = temp;

		// Now check parent
		itemIndex = parent(itemIndex);
	}
}
