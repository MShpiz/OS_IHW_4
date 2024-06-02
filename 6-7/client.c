#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <signal.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */
char children[RCVBUFSIZE];

char tmp[RCVBUFSIZE]; // оно глобальное, чтобы если пид не целиком попал в буфер
                      // он не потерялся
int c = 0;
void killChildren() {
  for (size_t i = 0; i < strlen(children); i++) {
    if (children[i] == '|' || children[i] == '-') {
      tmp[c] = '\0';
      int pid = atoi(tmp);
      kill(pid, SIGKILL);
      if (children[i] == '|') {
        printf("%d client is done\n", pid);
      } else {
        printf("%d client left because queue was full\n", pid);
      }

    } else {
      tmp[c++] = children[i];
    }
  }
  exit(0);
}

int sock = -1;

void my_handler(int nsig) {
  if (sock != -1) {
    //killChildren();
    close(sock);
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  (void)signal(SIGINT, my_handler);
  /* Socket descriptor */
  struct sockaddr_in echoServAddr; /* Echo server address */
  struct sockaddr_in fromAddr;     /* Source address of echo */
  unsigned short echoServPort;     /* Echo server port */
  unsigned int fromSize;           /* In-out of address size for recvfrom() */
  char *servIP;                    /* Server IP address (dotted quad) */
  char *echoString;                /* String to send to echo server */
  char echoBuffer[RCVBUFSIZE+1];     /* Buffer for echo string */
  unsigned int echoStringLen;      /* Length of string to echo */
  int respStringLen;               /* Length of received response */

  if ((argc < 3) || (argc > 4)) /* Test for correct number of arguments */
  {
    fprintf(stderr,
            "Usage: %s <Server IP> <Client generation time> [<Echo Port>]\n",
            argv[0]);
    exit(1);
  }

  servIP = argv[1];         /* First arg: server IP address (dotted quad) */
  int time = atoi(argv[2]); /* Second arg: time */

  if (argc == 4)
    echoServPort = atoi(argv[3]); /* Use given port, if any */
  else
    echoServPort = 7; /* 7 is the well-known port for the echo service */

  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("socket() failed");

  /* Construct the server address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
  echoServAddr.sin_family = AF_INET;              /* Internet address family */
  echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  echoServAddr.sin_port = htons(echoServPort);      /* Server port */

  /* Establish the connection to the echo server */
  if (connect(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    DieWithError("connect() failed");
  printf("parent pid %d \n", getpid());
  int child;

  for (;;) {
    child = fork();

    if (child == 0) {
      printf("client %d came in\n", getpid());
      for (int j = 0; j < RCVBUFSIZE; j++) {
        echoBuffer[j] = '\0';
      }
      sprintf(echoBuffer, "%d", getpid());
      int l = strlen(echoBuffer);
      echoBuffer[l] = '|';
      echoBuffer[l + 1] = '\0';
      echoStringLen = strlen(echoBuffer);
      /* Send the string to the server */
      // printf("send %s %d\n", echoBuffer, echoStringLen);
      if (sendto(sock, echoBuffer, echoStringLen, 0, (struct sockaddr *)
          &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");
      for(;;);

    } else {
      int child2 = fork();
      if (child2 == 0) {
        fromSize = sizeof(fromAddr);
        if ((respStringLen = recvfrom(sock, children, RCVBUFSIZE, 0,
            (struct sockaddr *) &fromAddr, &fromSize)) == echoStringLen)
            DieWithError("recvfrom() failed");

        if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
            fprintf(stderr,"Error: received a packet from unknown source.\n");
            exit(1);
        }

        killChildren();
        exit(0);
      }
    }
    sleep(time);
  }

  close(sock);
  exit(0);
}
