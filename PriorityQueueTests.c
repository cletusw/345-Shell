#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(3);
	assert("Q length error" && q->length == 0);
	
	PriorityQueueItem* qArray = q->first;
	enQ(q, 10, 100);
	assert("Insert error" && q->length == 1);

	enQ(q, 11, 101);
	assert("Insert error" && q->length == 2);

	enQ(q, 12, 102);
	assert("Insert error" && q->length == 3);
	
	print(q);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
