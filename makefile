CC=gcc
CFLAGS=-Wall

all: UDPEchoClient UDPEchoServer

UDPEchoClient: UDPEchoClient.o DieWithError.o 

UDPEchoServer: UDPEchoServer.o DieWithError.o 

DieWithError.o: DieWithError.c
	$(CC) $(CFLAGS) -c DieWithError.c

UDPEchoClient.o: UDPEchoClient.c
	$(CC) $(CFLAGS) -c UDPEchoClient.c

UDPEchoSever.o: UDPEchoSever.c 
	$(CC) $(CFLAGS) -c UDPEchoSever.c

clean:
	rm -f *.o *~ UDPEchoClient UDPEchoServer ClientFiles/*
