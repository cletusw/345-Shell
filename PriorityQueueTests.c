#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue(6);
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

	print(q);

	// Pop
	int data, priority;
	data = pop(q);
	assert("Pop error" && q->length == 4);
	assert("Pop error" && data == 8);
	assert("Pop error" && q->array[0].data == 2 && q->array[0].priority == 103);
	assert("Pop error" && q->array[1].data == 90 && q->array[1].priority == 101);
	assert("Pop error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("Pop error" && q->array[3].data == 10 && q->array[3].priority == 100);

	data = pop(q);
	assert("Pop error" && q->length == 3);
	assert("Pop error" && data == 2);
	assert("Pop error" && q->array[0].data == 11 && q->array[0].priority == 102);
	assert("Pop error" && q->array[1].data == 90 && q->array[1].priority == 101);
	assert("Pop error" && q->array[2].data == 10 && q->array[2].priority == 100);

	data = pop(q);
	assert("Pop error" && q->length == 2);
	assert("Pop error" && data == 11);
	assert("Pop error" && q->array[0].data == 90 && q->array[0].priority == 101);
	assert("Pop error" && q->array[1].data == 10 && q->array[1].priority == 100);

	data = pop(q);
	assert("Pop error" && q->length == 1);
	assert("Pop error" && data == 90);
	assert("Pop error" && q->array[0].data == 10 && q->array[0].priority == 100);

	data = pop(q);
	assert("Pop error" && q->length == 0);
	assert("Pop error" && data == 10);

	data = pop(q);
	assert("Pop error" && data == -1);
	assert("Pop error" && q->length == 0);

	// Test deQ
	enQ(q, 10, 100);
	enQ(q, 11, 102);
	enQ(q, 8, 105);
	enQ(q, 2, 103);
	enQ(q, 90, 101);
	enQ(q, 20, 98);
	assert("DeQ error" && q->length == 6);
	assert("DeQ error" && q->array[0].data == 8 && q->array[0].priority == 105);
	assert("DeQ error" && q->array[1].data == 2 && q->array[1].priority == 103);
	assert("DeQ error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("DeQ error" && q->array[3].data == 10 && q->array[3].priority == 100);
	assert("DeQ error" && q->array[4].data == 90 && q->array[4].priority == 101);
	assert("DeQ error" && q->array[5].data == 20 && q->array[5].priority == 98);

	print(q);

	deQ(q, 2);
	assert("DeQ error" && q->length == 5);
	assert("DeQ error" && q->array[0].data == 8 && q->array[0].priority == 105);
	assert("DeQ error" && q->array[1].data == 90 && q->array[1].priority == 101);
	assert("DeQ error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("DeQ error" && q->array[3].data == 10 && q->array[3].priority == 100);
	assert("DeQ error" && q->array[4].data == 20 && q->array[4].priority == 98);

	deQ(q, 90);
	assert("DeQ error" && q->length == 4);
	assert("DeQ error" && q->array[0].data == 8 && q->array[0].priority == 105);
	assert("DeQ error" && q->array[1].data == 10 && q->array[1].priority == 100);
	assert("DeQ error" && q->array[2].data == 11 && q->array[2].priority == 102);
	assert("DeQ error" && q->array[3].data == 20 && q->array[3].priority == 98);

	data = pop(q);
	assert("Pop error" && q->length == 3);
	assert("Pop error" && data == 8);

	data = pop(q);
	assert("Pop error" && q->length == 2);
	assert("Pop error" && data == 11);

	data = pop(q);
	assert("Pop error" && q->length == 1);
	assert("Pop error" && data == 10);

	data = pop(q);
	assert("Pop error" && q->length == 0);
	assert("Pop error" && data == 20);

	data = pop(q);
	assert("Pop error" && data == -1);
	assert("Pop error" && q->length == 0);

	enQ(q, 20, 98);
	data = pop(q);
	assert("Pop error" && q->length == 0);
	assert("Pop error" && data == 20);
	enQ(q, 20, 98);
	data = pop(q);
	assert("Pop error" && q->length == 0);
	assert("Pop error" && data == 20);
	enQ(q, 20, 98);
	data = pop(q);
	assert("Pop error" && q->length == 0);
	assert("Pop error" && data == 20);
	enQ(q, 20, 98);
	data = pop(q);
	assert("Pop error" && q->length == 0);
	assert("Pop error" && data == 20);

	print(q);
	
	freePriorityQueue(q);
	q = 0;

	return 0;
}
