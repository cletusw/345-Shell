#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(5);
	assert("Q length error" && q->length == 0);
	
	PriorityQueueItem* qArray = q->first;
	enQ(q, 10, 100);
	assert("Insert error" && q->length == 1);
	assert("Insert error" && qArray[0].data == 10 && qArray[0].priority == 100);

	enQ(q, 11, 102);
	assert("Insert error" && q->length == 2);
	assert("Insert error" && qArray[0].data == 11 && qArray[0].priority == 102);
	assert("Insert error" && qArray[1].data == 10 && qArray[1].priority == 100);

	enQ(q, 8, 105);
	assert("Insert error" && q->length == 3);
	assert("Insert error" && qArray[0].data == 8 && qArray[0].priority == 105);
	assert("Insert error" && qArray[1].data == 10 && qArray[1].priority == 100);
	assert("Insert error" && qArray[2].data == 11 && qArray[2].priority == 102);
	
	enQ(q, 2, 103);
	assert("Insert error" && q->length == 4);
	assert("Insert error" && qArray[0].data == 8 && qArray[0].priority == 105);
	assert("Insert error" && qArray[1].data == 2 && qArray[1].priority == 103);
	assert("Insert error" && qArray[2].data == 11 && qArray[2].priority == 102);
	assert("Insert error" && qArray[3].data == 10 && qArray[3].priority == 100);
	
	enQ(q, 90, 101);
	assert("Insert error" && q->length == 5);
	assert("Insert error" && qArray[0].data == 8 && qArray[0].priority == 105);
	assert("Insert error" && qArray[1].data == 2 && qArray[1].priority == 103);
	assert("Insert error" && qArray[2].data == 11 && qArray[2].priority == 102);
	assert("Insert error" && qArray[3].data == 10 && qArray[3].priority == 100);
	assert("Insert error" && qArray[4].data == 90 && qArray[4].priority == 101);
	print(q);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
