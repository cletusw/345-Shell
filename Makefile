shell: os345p1.c os345.c os345p2.c PriorityQueue.c
	gcc -g os345*.c PriorityQueue.c -o shell

clean:
	@rm -f *.o shell test

test:
	gcc -o test PriorityQueue*.c
	-valgrind --tool=memcheck --leak-check=full ./test

testShell: shell
	-valgrind --tool=memcheck --leak-check=full ./shell
