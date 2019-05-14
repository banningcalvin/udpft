CC=gcc
#CFLAGS=-Wall -g

all: client server

client: client.o DieWithError.o checksum.o

server: server.o DieWithError.o checksum.o

DieWithError.o: DieWithError.c
	$(CC) $(CFLAGS) -c DieWithError.c

checksum.o: checksum.c
	$(CC) $(CFLAGS) -c checksum.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o *~ client server ClientFiles/* core
