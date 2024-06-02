#include "TCPEchoServer.h"
#include "signal.h"
void ProcessMain(int servSock, int, int, int, struct sockaddr_in);         /* Main program of process */

int servSock;                    /* Socket descriptor for server*/
// void my_handler(int nsig) {
//   if (servSock != -1) {
//     close(servSock);
//   }
//   exit(0);
// }

int main(int argc, char *argv[])
{
    unsigned short echoServPort;     /* Server port */
    pid_t processID;                 /* Process ID */
    //unsigned int processLimit;       /* Number of child processes to create */
    unsigned int processCt;          /* Process counter */

    char *logIP;
    struct sockaddr_in logServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
      unsigned short logServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */

    if (argc != 6)     /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage:  %s <SERVER PORT> <WORK TIME> <QLen> <LOG SERVER IP> <LOG SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg: local port */

    int work_time = atoi(argv[2]);
    int q_len = atoi(argv[3]);
    if (q_len > 1000) {
        printf("Queue too long, must be <= 1000");
        exit(0);
    }
    logIP = argv[4];
    logServPort = atoi(argv[5]);

    int logSock = -1;
    if ((logSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

      /* Construct the server address structure */
      memset(&logServAddr, 0, sizeof(logServAddr)); /* Zero out structure */
    logServAddr.sin_family = AF_INET;              /* Internet address family */
        logServAddr.sin_addr.s_addr = inet_addr(logIP); /* Server IP address */
        logServAddr.sin_port = htons(logServPort);      /* Server port */

      /* Establish the connection to the echo server */
      if (connect(logSock, (struct sockaddr *)&logServAddr, sizeof(logServAddr)) < 0)
        DieWithError("connect() failed");
    
    servSock = CreateTCPServerSocket(echoServPort);
    //for (processCt=0; processCt < 10; processCt++)
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)  /* If this is the child process */
            ProcessMain(servSock, work_time, q_len, logSock, logServAddr);

    exit(0);  /* The children will carry on */
}

void ProcessMain(int servSock, int work_time, int q_len, int logSock, struct sockaddr_in logServAddr)
{
    int clntSock;                  /* Socket descriptor for client connection */

    for (;;)  /* Run forever */
    {
        // clntSock = AcceptTCPConnection(servSock);
        printf("with child process: %d\n", (unsigned int) getpid());
        HandleTCPClient(servSock, work_time, q_len, logSock, logServAddr);
    }
}
