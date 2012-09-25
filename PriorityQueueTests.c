#include <stdio.h>
#include "PriorityQueue.h"

int main(void) {
	PriorityQueueItem q[3];
	q[0].data = 10; q[0].priority = 100;
	q[1].data = 11; q[1].priority = 101;
	q[2].data = 12; q[2].priority = 102;
	
	print(q, 3);

	return 0;
}
