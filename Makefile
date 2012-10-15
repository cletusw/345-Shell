shell: os345p1.c os345.c os345p2.c PriorityQueue.c
	gcc -g os345*.c PriorityQueue.c -o shell

run: shell
	@./shell

clean:
	@rm -f *.o shell test

test: DeltaClockTests.c DeltaClock.c DeltaClock.h
	gcc -g -o test DeltaClock*.c
	-valgrind --tool=memcheck --leak-check=full ./test

testQueue:
	gcc -o test PriorityQueue*.c
	-valgrind --tool=memcheck --leak-check=full ./test

testShell: shell
	-valgrind --tool=memcheck --leak-check=full ./shell
