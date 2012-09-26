#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(5);
	assert("Q length error" && q->length == 0);
	
	enQ(q, 10, 100);
	assert("Insert error" && q->length == 1);
	assert("Insert error" && q->array[0].data == 10 && q->array[0].priority == 100);

	enQ(q, 11, 102);
	assert("Insert error" && q->length == 2);
	assert("Insert error" && q->array[0].data == 11 && q->array[0].priority == 102);
	assert("Insert error" && q->array[1].data == 10 && q->array[1].priority == 100);

	enQ(q, 8, 105);
	assert("Insert error" && q->length == 3);
	assert("Insert error" && q->array[0].data == 8 && q->array[0].priority == 105);
	assert("Insert error" && q->array[1].data == 10 && q->array[1].priority == 100);
	assert("Insert error" && q->array[2].data == 11 && q->array[2].priority == 102);
	
	enQ(q, 2, 103);
	assert("Insert error" && q->length == 4);
	assert("Insert error" && q->array[0].data == 8 && q->array[0].priority == 105);
	assert("Insert error" && q->array[1].data == 2 && q->array[1].priority == 103);
	assert("Insert error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("Insert error" && q->array[3].data == 10 && q->array[3].priority == 100);
	
	enQ(q, 90, 101);
	assert("Insert error" && q->length == 5);
	assert("Insert error" && q->array[0].data == 8 && q->array[0].priority == 105);
	assert("Insert error" && q->array[1].data == 2 && q->array[1].priority == 103);
	assert("Insert error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("Insert error" && q->array[3].data == 10 && q->array[3].priority == 100);
	assert("Insert error" && q->array[4].data == 90 && q->array[4].priority == 101);

	// Pop
	int data, priority;
	pop(q, &data, &priority);
	assert("Pop error" && q->length == 4);
	assert("Pop error" && data == 8 && priority == 105);
	assert("Pop error" && q->array[0].data == 2 && q->array[0].priority == 103);
	assert("Pop error" && q->array[1].data == 90 && q->array[1].priority == 101);
	assert("Pop error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("Pop error" && q->array[3].data == 10 && q->array[3].priority == 100);

	print(q);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
