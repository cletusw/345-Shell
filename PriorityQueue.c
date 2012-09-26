#include <stdlib.h>
#include <stdio.h>
#include "PriorityQueue.h"

#define parent(k) ((k-1)/2)
#define leftChild(k) (2*(k) + 1)

// Helper declarations
void exchange(PriorityQueue* q, int index1, int index2);
void swim(PriorityQueue* q, int itemIndex);
void sink(PriorityQueue* q, int itemIndex);

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

void pop(PriorityQueue* q, int* data, int* priority) {
	PriorityQueueItem* qArray = q->first;
	*data = qArray[0].data;
	*priority = qArray[0].priority;

	// Replace top with last item
	q->length--;
	qArray[0] = qArray[q->length];

	// Restore order
	sink(q, 0);

	// Prevent loitering
	qArray[q->length].data = 0;
	qArray[q->length].priority = 0;
}

// Helper definitions
void exchange(PriorityQueue* q, int index1, int index2) {
	PriorityQueueItem* qArray = q->first;
	PriorityQueueItem temp;

	temp = qArray[index1];
	qArray[index1] = qArray[index2];
	qArray[index2] = temp;
}

void swim(PriorityQueue* q, int itemIndex) {
	PriorityQueueItem* qArray = q->first;

	while (itemIndex > 0 && qArray[parent(itemIndex)].priority < qArray[itemIndex].priority) {
		// Swap item and parent
		exchange(q, parent(itemIndex), itemIndex);

		// Now check parent
		itemIndex = parent(itemIndex);
	}
}

void sink(PriorityQueue* q, int itemIndex) {
	PriorityQueueItem* qArray = q->first;

	while (leftChild(itemIndex) <= q->length) {
		int j = leftChild(itemIndex);
		if (j < q->length && qArray[j].priority < qArray[j+1].priority) {
			j++;
		}
		if (qArray[itemIndex].priority >= qArray[j].priority) {
			break;
		}
		exchange(q, itemIndex, j);
		itemIndex = j;
	}
}