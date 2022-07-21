CC = gcc
CFLAGS = -g -Wall

all: stree 

stree: stree.o
	$(CC) $(CFLAGS) -o stree stree.o

stree.o: stree.c 
	$(CC) $(CFLAGS) -c stree.c

clean:
	rm -f *.o  stree