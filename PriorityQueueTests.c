#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueue* q = newPriorityQueue();
	assert("PriorityQueue creation error" && q->head == NULL);
	
	enQ(q, 10, 100);
	assert("enQ error" && q->head != NULL);
	assert("enQ error" && q->head->data == 10);
	assert("enQ error" && q->head->priority == 100);

	enQ(q, 11, 102);
	enQ(q, 8, 105);	
	enQ(q, 2, 103);	
	enQ(q, 90, 101);

	print(q); printf("\n");

	// Pop
	int data, priority;
	data = pop(q);
	assert("Pop error" && data == 8);

	data = pop(q);
	assert("Pop error" && data == 2);

	data = pop(q);
	assert("Pop error" && data == 11);

	data = pop(q);
	assert("Pop error" && data == 90);

	data = pop(q);
	assert("Pop error" && data == 10);

	data = pop(q);
	assert("Pop error" && data == -1);

	print(q); printf("\n");

	// Test deQ
	enQ(q, 10, 100);
	enQ(q, 11, 102);
	enQ(q, 8, 105);
	enQ(q, 2, 103);
	enQ(q, 90, 101);
	enQ(q, 20, 98);
	
	print(q); printf("\n");

	deQ(q, 2);
	assert("DeQ error" && q->head != NULL);

	data = pop(q);
	assert("Pop error after deQ" && data == 8);
	data = pop(q);
	assert("Pop error after deQ" && data == 11);

	deQ(q, 90);
	assert("DeQ error" && q->head != NULL);

	data = pop(q);
	assert("Pop error after deQ" && data == 10);
	data = pop(q);
	assert("Pop error after deQ" && data == 20);
	data = pop(q);
	assert("Pop error after deQ" && data == -1);
	assert("Pop error after deQ" && q->head == NULL);

	deQ(q, 90);
	assert("DeQ error" && q->head == NULL);

	print(q); printf("\n");

	enQ(q, 10, 100);
	enQ(q, 11, 102);
	enQ(q, 8, 105);
	enQ(q, 2, 103);
	enQ(q, 90, 101);
	enQ(q, 20, 98);

	deQ(q, 90);
	assert("DeQ error" && q->head != NULL);

	data = pop(q);
	assert("Pop error" && data == 8);
	data = pop(q);
	assert("Pop error" && data == 2);
	data = pop(q);
	assert("Pop error" && data == 11);
	data = pop(q);
	assert("Pop error" && data == 10);
	data = pop(q);
	assert("Pop error" && data == 20);
	data = pop(q);
	assert("Pop error" && data == -1);

	print(q); printf("\n");


	enQ(q, 20, 98);
	assert("enQ error" && q->head != NULL);
	data = pop(q);
	assert("Pop error" && data == 20);
	assert("pop error" && q->head == NULL);

	enQ(q, 20, 98);
	assert("enQ error" && q->head != NULL);
	data = pop(q);
	assert("Pop error" && data == 20);
	assert("pop error" && q->head == NULL);

	enQ(q, 20, 98);
	assert("enQ error" && q->head != NULL);
	data = pop(q);
	assert("Pop error" && data == 20);
	assert("pop error" && q->head == NULL);

	print(q); printf("\n");

	// Pop and reQ
	enQ(q, 0, 98);
	enQ(q, 1, 100);
	enQ(q, 2, 100);
	enQ(q, 3, 100);
	enQ(q, 4, 100);

	print(q); printf("\n");

	data = pop(q);
	assert("Pop error" && data == 1);
	enQ(q, 1, 100);

	data = pop(q);
	assert("Pop error" && data == 2);
	enQ(q, 2, 100);

	data = pop(q);
	assert("Pop error" && data == 3);
	enQ(q, 3, 100);

	data = pop(q);
	assert("Pop error" && data == 4);
	enQ(q, 4, 100);

	data = pop(q);
	assert("Pop error" && data == 1);
	enQ(q, 1, 100);

	data = pop(q);
	assert("Pop error" && data == 2);
	enQ(q, 2, 100);

	data = pop(q);
	assert("Pop error" && data == 3);
	enQ(q, 3, 100);

	print(q); printf("\n");
	
	deletePriorityQueue(q);
	q = 0;

	return 0;
}
