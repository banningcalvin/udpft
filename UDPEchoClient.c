#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



#define SEGSIZE 255 /* Maximum data segment size */

void DieWithError(char *errorMessage); /* Error handling function */



int main(int argc, char *argv[])
{
  int sock; /*Socket descriptor*/
  struct sockaddr_in servAddr; /* server address */
  struct sockaddr_in fromAddr; /* source address */
  unsigned short servPort;
  unsigned int fromSize;
  char *servIP;
  char buffer[SEGSIZE+1];
  char *fileName = "example.txt";
  unsigned int fileNameLen;
  int respStringLen;

  /* Test for correct number of arguments */
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
    exit(1);
  }

  printf("Starting client.\n");
  servIP = argv[1];
  servPort = atoi(argv[2]);
  
  if((fileNameLen = strlen(fileName)) > SEGSIZE) /* Check input length */
    DieWithError("Filename too long");

  /* Create a datagram/UDP socket */
  if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0)
    DieWithError("socket() failed");

  /*Construct the server address structure*/
  memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
  servAddr.sin_family = AF_INET;   /* Internet addr family */
  servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
  servAddr.sin_port = htons(servPort);  /* Server port */


  /*send the string to the server*/
  if(sendto(sock, fileName, fileNameLen, 0, (struct sockaddr *)&servAddr, sizeof(servAddr))!=fileNameLen)
    DieWithError("send() sent a different number of bytes than expected");

  /*Receive a response*/
  fromSize = sizeof(fromAddr);
  if((respStringLen = recvfrom(sock, buffer, SEGSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) != fileNameLen)
    DieWithError("recvfrom() failed") ;

  if(servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    fprintf(stderr,"Error: received a packet from unknown source.\nCheck that the IP addresses are identical (0.0.0.0 will not match 127.0.0.1).\n");
    exit(1);
  }


  /* null-terminate the received data */
  buffer[respStringLen] = '\0' ;
  printf("Received: %s\n", buffer); /* Print the echoed arg */
  close(sock);
  exit(0);
}
