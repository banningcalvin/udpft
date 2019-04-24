#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEGSIZE 255 /* Maximum data segment size */



void DieWithError(char *errorMessage); /* Error handling function */

/* Sets fileName */
void setFileName(char *fileName) {
  printf("Input file name: ");
  scanf("%[^\n]%*c", fileName); 
}



int main(int argc, char *argv[])
{
  int sock; /*Socket descriptor*/
  struct sockaddr_in servAddr; /* server address */
  struct sockaddr_in fromAddr; /* source address */
  unsigned short servPort;
  unsigned int fromSize;
  char *servIP;
  char buffer[SEGSIZE+1];
  char *fileName;
  int respStringLen;

  /* Test for correct number of arguments */
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
    exit(1);
  }

  printf("Starting client.\n");
  fileName = malloc(sizeof(char)*(SEGSIZE+1));

  /* start by asking for the filename */
  setFileName(fileName);
  
  /* Create a datagram/UDP socket */
  servIP = argv[1];
  servPort = atoi(argv[2]);
  if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0)
    DieWithError("socket() failed");
  
  /*Construct the server address structure*/
  memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
  servAddr.sin_family = AF_INET;   /* Internet addr family */
  servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
  servAddr.sin_port = htons(servPort);  /* Server port */

  /*send the string to the server*/
  if(sendto(sock, fileName, strlen(fileName), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) != strlen(fileName))
    DieWithError("send() sent a different number of bytes than expected");
  else
    printf("File '%s' requested from %s:%d\n", fileName, servIP, servPort);



  
  /*Receive a response*/
  fromSize = sizeof(fromAddr);
  if((respStringLen = recvfrom(sock, buffer, SEGSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize)) != strlen(fileName))
    DieWithError("recvfrom() failed") ;

  if(servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    fprintf(stderr,"Error: received a packet from unknown source.\nCheck that the IP addresses are identical (0.0.0.0 will not match 127.0.0.1).\n");
    exit(1);
  }


  /* null-terminate the received data */
  buffer[respStringLen] = '\0' ;
  printf("Received: %s\n", buffer); /* Print the echoed arg */

  printf("Closing client.\n");
  close(sock);
  free(fileName);
  exit(0);
}
