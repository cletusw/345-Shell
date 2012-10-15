#include <stdio.h>
#include <assert.h>
#include "DeltaClock.h"

void callback0(void);
void callback1(void);
void callback2(void);
void callback3(void);
void callback4(void);
void callback5(void);
void callback6(void);
void callback7(void);
void callback8(void);
int called[9];

int main(void) {
	DeltaClock* dc = newDeltaClock();
	assert("DeltaClock creation error" && dc->head == NULL);
	
	print(dc);

	insert(dc, 2, &callback0);
	insert(dc, 5, &callback1);
	insert(dc, 10, &callback2);
	insert(dc, 9, &callback3);
	insert(dc, 10, &callback4);
	insert(dc, 11, &callback5);
	insert(dc, 2, &callback6);
	insert(dc, 1, &callback7);
	insert(dc, 3, &callback8);

	print(dc);

	assert ("tic Error" && !called[7]);
	tic(dc);
	assert ("tic Error" && called[7]);

	assert ("tic Error" && !called[0]);
	assert ("tic Error" && !called[6]);
	tic(dc);
	assert ("tic Error" && called[0]);
	assert ("tic Error" && called[6]);

	assert ("tic Error" && !called[8]);
	tic(dc);
	assert ("tic Error" && called[8]);

	assert ("tic Error" && !called[1]);
	tic(dc);
	tic(dc);
	assert ("tic Error" && called[1]);

	assert ("tic Error" && !called[3]);
	tic(dc);tic(dc);tic(dc);tic(dc);
	assert ("tic Error" && called[3]);

	assert ("tic Error" && !called[2]);
	assert ("tic Error" && !called[4]);
	tic(dc);
	assert ("tic Error" && called[2]);
	assert ("tic Error" && called[4]);

	assert ("tic Error" && !called[5]);
	tic(dc);
	assert ("tic Error" && called[5]);

	print(dc);
	
	deleteDeltaClock(dc);
	dc = 0;

	return 0;
}

void callback0(void) {
	printf("callback0\n");
	called[0]++;
}

void callback1(void) {
	printf("callback1\n");
	called[1]++;
}

void callback2(void) {
	printf("callback2\n");
	called[2]++;
}

void callback3(void) {
	printf("callback3\n");
	called[3]++;
}

void callback4(void) {
	printf("callback4\n");
	called[4]++;
}

void callback5(void) {
	printf("callback5\n");
	called[5]++;
}

void callback6(void) {
	printf("callback6\n");
	called[6]++;
}

void callback7(void) {
	printf("callback7\n");
	called[7]++;
}

void callback8(void) {
	printf("callback8\n");
	called[8]++;
}