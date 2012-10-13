#include <stdio.h>
#include <assert.h>
#include "DeltaClock.h"

void callback1(void);
void callback2(void);
void callback3(void);

int main(void) {
	DeltaClock* dc = newDeltaClock();
	assert("DeltaClock creation error" && dc->head == NULL);
	
	print(dc);

	insert(dc, 2, &callback1);
	insert(dc, 5, &callback2);
	insert(dc, 8, &callback3);

	print(dc);
	
	deleteDeltaClock(dc);
	dc = 0;

	return 0;
}

void callback1(void) {
	printf("callback1");
}

void callback2(void) {
	printf("callback1");
}

void callback3(void) {
	printf("callback1");
}