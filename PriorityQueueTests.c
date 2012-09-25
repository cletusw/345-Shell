#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(3);
	
	assert("Q length error" && q->length == 3);
	
	PriorityQueueItem* qi = q->first;
	qi[0].data = 10; qi[0].priority = 100;
	qi[1].data = 11; qi[1].priority = 101;
	qi[2].data = 12; qi[2].priority = 102;
	
	print(qi, 3);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
