shell: os345p1.c os345.c os345p2.c PriorityQueue.c
	gcc os345*.c PriorityQueue.c -o shell

clean:
	@rm -f *.o shell test

test:
	gcc -o test PriorityQueue*.c
	./test
	-valgrind --tool=memcheck ./test
