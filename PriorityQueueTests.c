#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(3);
	assert("Q length error" && q->length == 0);
	
	PriorityQueueItem* qArray = q->first;
	enQ(q, 10, 100);
	assert("Insert error" && q->length == 1);
	assert("Insert error" && qArray[0].data == 10 && qArray[0].priority == 100);

	enQ(q, 11, 101);
	assert("Insert error" && q->length == 2);
	assert("Insert error" && qArray[0].data == 11 && qArray[0].priority == 101);
	assert("Insert error" && qArray[1].data == 10 && qArray[1].priority == 100);

	enQ(q, 8, 102);
	assert("Insert error" && q->length == 3);
	assert("Insert error" && qArray[0].data == 8 && qArray[0].priority == 102);
	assert("Insert error" && qArray[1].data == 11 && qArray[1].priority == 101);
	assert("Insert error" && qArray[2].data == 10 && qArray[2].priority == 100);
	
	print(q);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
