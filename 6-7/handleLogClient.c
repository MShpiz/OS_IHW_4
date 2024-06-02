#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

void DieWithError(char *errorMessage); /* Error handling function */

const int RCVBUFSIZE = 32;

void HandleLogClient(int sock) {
  char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
  int recvMsgSize;
  struct sockaddr_in echoClntAddr; /* Client address */
  unsigned int cliAddrLen;         /* Length of incoming message */
  cliAddrLen = sizeof(echoClntAddr);

  /* Block until receive message from a client */
  if ((recvMsgSize = recvfrom(sock, echoBuffer, RCVBUFSIZE, 0,
      (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
      DieWithError("recvfrom() failed");

  printf("info from server %s: \"%s\" \n", inet_ntoa(echoClntAddr.sin_addr), echoBuffer);
}