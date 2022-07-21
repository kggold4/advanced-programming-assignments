CC = gcc
CFLAGS = -g -Wall

all: slast 

slast: slast.o
	$(CC) $(CFLAGS) -o slast slast.o

slast.o: slast.c 
	$(CC) $(CFLAGS) -c slast.c

clean:
	rm -f *.o  slast