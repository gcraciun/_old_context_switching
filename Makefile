all:
	gcc -Wall -o context context.c
	gcc -Wall -o context2 context2.c
clean:
	rm -f *~ *.o context context2
