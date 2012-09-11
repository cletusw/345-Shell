shell: os345.o os345p1.o
	gcc -o $@ os345.o os345p1.o
	chmod ugo+x $@

os345.o: os345.c os345.h
	gcc -o $@ -c os345.c

os345p1.o: os345p1.c
	gcc -o $@ -c os345p1.c

clean:
	@rm -f *.o shell