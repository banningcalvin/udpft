/* This file contains the source code for the server. It receives a file
 * name from a client. If found, it notifies the client and then begins sending
 * the data to the client. It then sends a checksum for validation. If not
 * found, it notifies the client. If not found, it notifies the client.
 * Unlike the client, the server does not need to be restarted for every
 * file.
 */

#include <stdio.h>      /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h>  /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEGSIZE 255 /* Maximum data segment size */

/* struct representing the packets to be sent, contains a checksum and data */
struct message
{
    char data[SEGSIZE];
    unsigned int checksum;
};
/* acks are distinct. they have an int which is 0 if everything is ok, */
/* and a sequence number */
struct ackmsg
{
    int isack;
};

void DieWithError(char *errorMessage);              /* Error handling function */
unsigned int calculateChecksum(char *path);         /* Checksum calculator */
unsigned int calculateStringChecksum(char *buffer); /* String checksum calculator */

/* makes len indices of char* b null terminators. Used to blank strings */
void blankBuffer(char *buffer, int len)
{
    for (int i = 0; i < len; i++)
        buffer[i] = '\0';
}

/* Fills buffer b of length len with data from the filepointer file */
/* returns 1 if EOF reached, 0 otherwise */
int prepBuffer(FILE *file, char *buffer, int len)
{
    char next; /* next character read */
    for (int i = 0; i < len; i++)
    {
        next = fgetc(file);
        if (next == EOF)
        { /* if EOF reached, null terminate and return 1 */
            buffer[i] = '\0';
            return 1;
        }
        else
        { /* else continue read */
            buffer[i] = next;
        }
    }
    return 0;
}

/* function used to determine if a packet should have simulated biterrors */
/* returns 1 if a packet should have simulated biterrors */
/* returns 0 if a packet should not have simulated biterrors */
int hasError(float p)
{
    return (p > (((float)rand()) / ((float)RAND_MAX)));
}

int main(int argc, char *argv[])
{
    int sock;                    /* Socket */
    struct sockaddr_in servAddr; /* Local address */
    struct sockaddr_in clntAddr; /* Client address */
    unsigned int cliAddrLen;     /* Length of incoming message */
    char buffer[SEGSIZE];        /* Buffer for string */
    char filePath[SEGSIZE];      /* Buffer for filePath */
    unsigned short servPort;     /* Server port */
    int recvMsgSize;             /* Size of received message */
    FILE *file;                  /* pointer to requested file */
    unsigned int checksum;       /* sum of bytes of entire file. used for final validation  */
    float probability;           /* probability for biterror */
    int EOFreached;              /* 1 if EOF reached, 0 if not reached */
    unsigned int windowSize;     /* window size, set by sender */
    unsigned int seq;            /* sequence number of current packet */
    unsigned int ack;            /* ack number */
    struct message msg;          /*message struct*/

    /* Test for correct number of arguments */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <Server Port> <Biterror Probability>\n", argv[0]);
        exit(1);
    }

    printf("Starting server.\n");
    servPort = atoi(argv[1]);    /* First arg: local port */
    probability = atof(argv[2]); /* Second arg: error probability */
    printf("Server Port: %d\n", servPort);
    printf("Bit Error Probability: %f\n", probability);

    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));       /* Zero out structure */
    servAddr.sin_family = AF_INET;                /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    servAddr.sin_port = htons(servPort);          /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");

    /* Set the size of the in-out parameter */
    // Note to self: this was originally in the for loop, does it belong there?
    cliAddrLen = sizeof(clntAddr);

    for (;;)
    { /* run until break */
        /* reset the buffers */
        blankBuffer(buffer, SEGSIZE);
        blankBuffer(filePath, SEGSIZE);
        strcpy(filePath, "./ServerFiles/");

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, buffer, SEGSIZE, 0, (struct sockaddr *)&clntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");

        /* exit on EXIT message */
        if (!strcmp(buffer, "EXIT"))
            break;

        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

        /* Check that the file exists and is readable */
        if (access(strcat(filePath, buffer), R_OK) != -1)
        {
            /* The file exists and is readable */
            printf("Requested file '%s' exists.\n", filePath);
            /* Send OK to the client */
            if (sendto(sock, "200 - FILE FOUND", strlen("200 - FILE FOUND"), 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr)) != strlen("200 - FILE FOUND"))
                DieWithError("sendto() sent a different number of bytes than expected");
            else
            { /* 200 sent to client. Now send the file after getting the window size */

                recvfrom(sock, &windowSize, sizeof(unsigned int), 0, (struct sockaddr *)&clntAddr, &cliAddrLen);
                printf("Window size recieved from client: %d\n", windowSize);

                file = fopen(filePath, "r");

                printf("'%s' opened. Preparing to buffer and send.\n", filePath);

                /* File transfer until break*/
                EOFreached = 0;
                seq = 0;
                while (!EOFreached)
                {

                    blankBuffer(buffer, SEGSIZE);

                    /* EOF reached. Send and break */
                    if (prepBuffer(file, buffer, SEGSIZE))
                    {
                        sendto(sock, buffer, SEGSIZE, 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr));
                        EOFreached = 1;
                    }
                    else
                    { /* send data as normal */
                        /*build packet, then send packet */
                        /* if biterror, send bad packet */
                        if (hasError(probability))
                        {
                            /*this packet is a biterror*/
                            sendto(sock, buffer, SEGSIZE, 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr));
                        }
                        sendto(sock, buffer, SEGSIZE, 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr));
                    }
                }

                fclose(file);

                printf("Getting ready to send checksum\n");
                checksum = calculateChecksum(filePath);
                sendto(sock, &checksum, sizeof(unsigned int), 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr));
                printf("Checksum sent.\n");
            }
        }
        else
        {
            /* The file does not exist or is not readable */
            printf("Requested file '%s' does not exist.\n", filePath);
            if (sendto(sock, "404 - FILE NOT FOUND", strlen("404 - FILE NOT FOUND"), 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr)) != strlen("404 - FILE NOT FOUND"))
                DieWithError("sendto() sent a different number of bytes than expected");
        }
        printf("Ready to send next file.\n");
    }

    printf("Closing socket.\n");
    close(sock);
    printf("Shutting down server.\n");
    exit(0);
}
