CC=gcc
CFLAGS=
OPT=-O3 -msse -funroll-loops

test: swifft.o test.c setup.o swifft.h
	${CC} ${CFLAGS} swifft.o  setup.o test.c -o test

setup.o: setup.c swifft.h
	${CC} ${CFLAGS} -c setup.c

swifft.o: swifft.c
	${CC} ${CFLAGS} ${OPT} -c swifft.c

clean: 
	rm -f *.o test
