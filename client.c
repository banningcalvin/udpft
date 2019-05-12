/* This file contains the source code for the client. It sends a file
 * request to the server. If not found, it exits. If it is, it downloads
 * the file, validates it with a checksum, and exits.
 */

#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEGSIZE 255 /* Maximum data segment size */



void DieWithError(char *errorMessage); /* Error handling function */
unsigned int calculateChecksum(char* path); /* Checksum calculator */

/* Sets fileName */
void setFileName(char *fileName) {
  printf("Input file name: ");
  scanf("%[^\n]%*c", fileName); 
}

/* makes len indices of char* b null terminators. Used to blank strings */
void blankBuffer(char* buffer, int len) {
  for(int i = 0; i < len; i++)
    buffer[i] = '\0';
}



int main(int argc, char *argv[])
{
  int sock; /*Socket descriptor*/
  struct sockaddr_in servAddr; /* server address */
  struct sockaddr_in fromAddr; /* source address */
  unsigned short servPort;
  unsigned int fromSize;
  char *servIP;
  char *buffer;
  char *fileName;
  FILE* file;
  unsigned int servChecksum;
  unsigned int clntChecksum;
  int windowSize;

  /* Test for correct number of arguments */
  if(argc != 4) {
    fprintf(stderr, "Usage: %s <Server IP> <Server Port> <Window Size>\n", argv[0]);
    exit(1);
  }

  printf("Starting client.\n");
  fileName = malloc(sizeof(char)*(SEGSIZE+1));
  buffer = malloc(sizeof(char)*(SEGSIZE+1));
  
  /* Create a datagram/UDP socket */
  servIP = argv[1];
  servPort = atoi(argv[2]);
  windowSize = atoi(argv[3]);
  printf("Target IP: %s\n", servIP);
  printf("Target Port: %d\n", servPort);
  printf("Window Size: %d\n", windowSize);
  
  if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0)
    DieWithError("socket() failed");
  
  /*Construct the server address structure*/
  memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
  servAddr.sin_family = AF_INET;   /* Internet addr family */
  servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
  servAddr.sin_port = htons(servPort);  /* Server port */



  
  /* start by asking for the filename */
  setFileName(fileName);
  
  /*send the string to the server*/
  if(sendto(sock, fileName, strlen(fileName), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) != strlen(fileName))
    DieWithError("send() sent a different number of bytes than expected");
  else
    printf("Sent request for file '%s' from %s:%d\n", fileName, servIP, servPort);



  /*Receive a response*/
  fromSize = sizeof(fromAddr);
  recvfrom(sock, buffer, SEGSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize);

  if(servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    fprintf(stderr,"Error: received a packet from unknown source.\nCheck that the IP addresses are identical (0.0.0.0 will not match 127.0.0.1).\n");
    exit(1);
  }


  
  if(strcmp("200 - FILE FOUND", buffer) == 0) {
    printf("Received: %s\n", buffer); /* Print the echoed arg */

    /* opening file to write, and recv and write messages into the buffer, then to the file */
    printf("Download beginning. Opening file.\n");
    file = fopen("./ClientFiles/download.txt","w");
    
    for(;;) {
      blankBuffer(buffer, SEGSIZE+1);
      recvfrom(sock, buffer, SEGSIZE, 0, (struct sockaddr *) &fromAddr, &fromSize);
      fputs(buffer,file);
      if(buffer[SEGSIZE - 1] == '\0') /* This segment had null bytes, EOF was reached */
	break;
    }
    
    printf("Download finished. Closing file. See ./ClientFiles/download.txt\n");
    fclose(file);
    printf("Preparing to validate file with checksum.\n");
    recvfrom(sock, &servChecksum, sizeof(unsigned int), 0, (struct sockaddr *) &fromAddr, &fromSize);
    clntChecksum = calculateChecksum("./ClientFiles/download.txt");
    printf("server checksum: %u\n", servChecksum);
    printf("client checksum: %u\n", clntChecksum);
    if(servChecksum == clntChecksum)
      printf("Checksums match.\n");
    else
      printf("Checksums differ. Something went wrong.\n");

    
  } else {
    printf("Received: %s\n", buffer); /* Print the echoed arg */
  }

  
  

  printf("Closing client.\n");
  close(sock);
  free(fileName);
  free(buffer);
  exit(0);
}
