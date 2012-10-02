#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "PriorityQueue.h"

// Public API
PriorityQueue* newPriorityQueue() {
	PriorityQueue* q = (PriorityQueue*) malloc(sizeof(PriorityQueue));
	q->head = NULL;
	return q;
}

void deletePriorityQueue(PriorityQueue* q) {
	assert("NULL PriorityQueue*" && q);
	PriorityQueueItem* cur = q->head;
	PriorityQueueItem* temp;

	while (cur) {
		temp = cur;
		cur = cur->next;
		free(temp);
	}

	free(q);
}

void print(PriorityQueue* q) {
	printf("::");

	PriorityQueueItem* cur = q->head;
	
	while (cur) {
		printf("{%d, %d} ", cur->data, cur->priority);
		cur = cur->next;
	}
	
	printf("::\n");
}

void enQ(PriorityQueue* q, int data, int priority) {
	PriorityQueueItem* item = (PriorityQueueItem*) malloc(sizeof(PriorityQueueItem));
	item->data = data;
	item->priority = priority;

	if (!q->head || priority > q->head->priority) {
		item->next = q->head;
		q->head = item;
		return;
	}
	
	PriorityQueueItem* cur = q->head;
	while (cur) {
		if (!cur->next || priority > cur->next->priority) {
			item->next = cur->next;
			cur->next = item;
			return;
		}
		cur = cur->next;
	}
}

int pop(PriorityQueue* q) {
	if (!q->head) {
		return EMPTY_QUEUE;
	}
	else {
		PriorityQueueItem* temp = q->head;
		int data = temp->data;
		q->head = temp->next;
		free(temp);
		return data;
	}
}

void deQ(PriorityQueue* q, int data) {
	PriorityQueueItem* cur = q->head;

	if (!cur) {
		return;
	}

	if (cur->data == data) {
		q->head = q->head->next;
		free(cur);
		return;
	}
	
	while (cur->next) {
		if (cur->next->data == data) {
			PriorityQueueItem* temp = cur->next;
			cur->next = cur->next->next;
			free(temp);
			return;
		}
	}
}
