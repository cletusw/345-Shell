#include <stdio.h>
#include <assert.h>
#include "DeltaClock.h"

void callback1(void);
void callback2(void);
void callback3(void);
void callback4(void);
void callback5(void);
void callback6(void);
void callback7(void);
void callback8(void);
void callback9(void);

int main(void) {
	DeltaClock* dc = newDeltaClock();
	assert("DeltaClock creation error" && dc->head == NULL);
	
	print(dc);

	insert(dc, 2, &callback1);
	insert(dc, 5, &callback2);
	insert(dc, 20, &callback3);
	insert(dc, 19, &callback4);
	insert(dc, 20, &callback4);
	insert(dc, 21, &callback5);
	insert(dc, 2, &callback6);
	insert(dc, 1, &callback7);
	insert(dc, 3, &callback8);

	print(dc);
	
	deleteDeltaClock(dc);
	dc = 0;

	return 0;
}

void callback1(void) {
	printf("callback1");
}

void callback2(void) {
	printf("callback2");
}

void callback3(void) {
	printf("callback3");
}

void callback4(void) {
	printf("callback4");
}

void callback5(void) {
	printf("callback5");
}

void callback6(void) {
	printf("callback6");
}

void callback7(void) {
	printf("callback7");
}

void callback8(void) {
	printf("callback8");
}

void callback9(void) {
	printf("callback9");
}