#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(3);
	
	assert("Q length error" && q->length == 0);
	
	enQ(q, 10, 100);
	enQ(q, 11, 101);
	enQ(q, 12, 102);
	
	PriorityQueueItem* qArray = q->first;
	assert("Insert error" && qArray[0].data == 10 && qArray[0].priority == 100);
	assert("Insert error" && qArray[1].data == 11 && qArray[1].priority == 101);
	assert("Insert error" && qArray[2].data == 12 && qArray[2].priority == 102);
	print(q);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
