#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "DeltaClock.h"

DeltaClock* newDeltaClock() {
	DeltaClock* dc = (DeltaClock*) malloc(sizeof(DeltaClock));
	dc->head = NULL;
	return dc;
}

void deleteDeltaClock(DeltaClock* dc) {
	assert("NULL DeltaClock*" && dc);
	DeltaClockItem* cur = dc->head;
	DeltaClockItem* temp;

	while (cur) {
		temp = cur;
		cur = cur->next;
		free(temp);
	}

	free(dc);
}

void printClock(DeltaClock* dc) {
	printf("**");

	DeltaClockItem* cur = dc->head;
	
	while (cur) {
		printf("{%d} ", cur->tics);
		cur = cur->next;
	}
	
	printf("**\n");
}

void insert(DeltaClock* dc, int tics, Semaphore* event) {
	DeltaClockItem* item = (DeltaClockItem*) malloc(sizeof(DeltaClockItem));
	item->event = event;
	item->next = NULL;

	if (!dc->head) {
		item->tics = tics;
		dc->head = item;
	}
	else if (tics < dc->head->tics) {
		item->tics = tics;
		dc->head->tics -= tics;
		item->next = dc->head;
		dc->head = item;
	}
	else {
		int diff = tics - dc->head->tics;
		DeltaClockItem* prev = dc->head;
		DeltaClockItem* cur = dc->head->next;

		while (1) {
			if (!cur) {
				prev->next = item;
				item->tics = diff;
				break;
			}

			diff -= cur->tics;

			if (diff < 0) {
				item->tics = diff + cur->tics;
				cur->tics = -diff;
				item->next = cur;
				prev->next = item;
				break;
			}

			prev = prev->next;
			cur = cur->next;
		}
	}
}

void tic(DeltaClock* dc) {
	if (dc->head) {
		dc->head->tics--;
		while (dc->head && dc->head->tics == 0) {
			semSignal(dc->head->event);
			DeltaClockItem* oldHead = dc->head;
			dc->head = dc->head->next;
			free(oldHead);
		}
	}
}
