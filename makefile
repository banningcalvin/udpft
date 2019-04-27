CC=gcc
CFLAGS=-Wall -g

all: client server

client: client.o DieWithError.o 

server: server.o DieWithError.o 

DieWithError.o: DieWithError.c
	$(CC) $(CFLAGS) -c DieWithError.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o *~ client server ClientFiles/* core
