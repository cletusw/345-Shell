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
	q->array = (PriorityQueueItem*) malloc(sizeof(PriorityQueueItem) * length);
	return q;
}

void freePriorityQueue(PriorityQueue* q) {
	free(q->array);
	free(q);
}

void print(PriorityQueue* q) {
	int i;
	int length = q->length;
	
	for (i = 0; i < length; i++) {
		printf("{%d, %d} ", q->array[i].data, q->array[i].priority);
	}
	
	printf("\n");
}

void enQ(PriorityQueue* q, int data, int priority) {
	int newItemIndex = q->length;
	PriorityQueueItem* item = &(q->array[newItemIndex]);
	item->data = data;
	item->priority = priority;
	q->length++;
	swim(q, newItemIndex);
}

void pop(PriorityQueue* q, int* data, int* priority) {
	*data = q->array[0].data;
	*priority = q->array[0].priority;

	// Replace top with last item
	q->length--;
	q->array[0] = q->array[q->length];

	// Restore order
	sink(q, 0);

	// Prevent loitering
	q->array[q->length].data = 0;
	q->array[q->length].priority = 0;
}

// Helper definitions
void exchange(PriorityQueue* q, int index1, int index2) {
	PriorityQueueItem temp;

	temp = q->array[index1];
	q->array[index1] = q->array[index2];
	q->array[index2] = temp;
}

void swim(PriorityQueue* q, int itemIndex) {
	while (itemIndex > 0 && q->array[parent(itemIndex)].priority < q->array[itemIndex].priority) {
		// Swap item and parent
		exchange(q, parent(itemIndex), itemIndex);

		// Now check parent
		itemIndex = parent(itemIndex);
	}
}

void sink(PriorityQueue* q, int itemIndex) {
	while (leftChild(itemIndex) <= q->length) {
		int j = leftChild(itemIndex);
		if (j < q->length && q->array[j].priority < q->array[j+1].priority) {
			j++;
		}
		if (q->array[itemIndex].priority >= q->array[j].priority) {
			break;
		}
		exchange(q, itemIndex, j);
		itemIndex = j;
	}
}