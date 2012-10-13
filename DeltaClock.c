#include <stdlib.h>
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

void print(DeltaClock* dc) {
	printf("**");

	DeltaClockItem* cur = dc->head;
	
	while (cur) {
		printf("{%d} ", cur->tics);
		cur = cur->next;
	}
	
	printf("**\n");
}