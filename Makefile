shell: os345p1.c os345.c
	gcc *.c -o shell

clean:
	@rm -f *.o shell
