shell: os345p1.c os345.c
	gcc *.c -o shell

clean:
	@rm -f *.o shell test

test:
	gcc -o test PriorityQueue*.c
	./test
	-valgrind --tool=memcheck test
