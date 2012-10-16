#include <stdio.h>
#include <assert.h>
#include "DeltaClock.h"

Semaphore events[9];
int called[9];

#define TIC(dc) printf("tic\n");tic(dc)

int main(void) {
	int i;
	for (i = 0; i < 9; i++) {
		events[i] = i;
	}

	DeltaClock* dc = newDeltaClock();
	assert("DeltaClock creation error" && dc->head == NULL);
	
	print(dc);

	insert(dc, 2, &events[0]);
	insert(dc, 5, &events[1]);
	insert(dc, 10, &events[2]);
	insert(dc, 9, &events[3]);
	insert(dc, 10, &events[4]);
	insert(dc, 11, &events[5]);
	insert(dc, 2, &events[6]);
	insert(dc, 1, &events[7]);
	insert(dc, 3, &events[8]);

	print(dc);

	assert ("tic Error" && !called[7]);
	TIC(dc);
	assert ("tic Error" && called[7]);

	assert ("tic Error" && !called[0]);
	assert ("tic Error" && !called[6]);
	TIC(dc);
	assert ("tic Error" && called[0]);
	assert ("tic Error" && called[6]);

	assert ("tic Error" && !called[8]);
	TIC(dc);
	assert ("tic Error" && called[8]);

	assert ("tic Error" && !called[1]);
	TIC(dc);
	TIC(dc);
	assert ("tic Error" && called[1]);

	assert ("tic Error" && !called[3]);
	TIC(dc);TIC(dc);TIC(dc);TIC(dc);
	assert ("tic Error" && called[3]);

	assert ("tic Error" && !called[2]);
	assert ("tic Error" && !called[4]);
	TIC(dc);
	assert ("tic Error" && called[2]);
	assert ("tic Error" && called[4]);

	assert ("tic Error" && !called[5]);
	TIC(dc);
	assert ("tic Error" && called[5]);

	print(dc);
	
	deleteDeltaClock(dc);
	dc = 0;

	return 0;
}

void semSignal(Semaphore* event) {
	printf("semSignal: %d\n", *event);
	called[*event]++;
}
