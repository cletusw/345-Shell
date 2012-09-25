#include <stdio.h>
#include "PriorityQueue.h"

void print(PriorityQueueItem q[], int length) {
	int i;
	
	for (i = 0; i < length; i++) {
		printf("{%d, %d} ", q[i].data, q[i].priority);
	}
	
	printf("\n");
}
