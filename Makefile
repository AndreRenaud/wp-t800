CFLAGS=-g -Wall -pipe

default: wp-test

%.o: %.c wp-t800.h
	$(CC) -c -o $@ $< $(CFLAGS)

wp-test: wp-test.o wp-t800.o
	$(CC) -o $@ wp-test.o wp-t800.o $(LFLAGS)

clean:
	rm -f *.o wp-test
