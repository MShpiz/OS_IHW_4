#include "TCPEchoServer.h"
#include "signal.h"
void ProcessMain(int servSock, int, int);         /* Main program of process */

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

    if (argc != 4)     /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage:  %s <SERVER PORT> <WORK TIME> <QLen>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg: local port */

    int work_time = atoi(argv[2]);
    int q_len = atoi(argv[3]);
    if (q_len > 1000) {
        printf("Queue too long, must be <= 1000");
        exit(0);
    }
    
    servSock = CreateTCPServerSocket(echoServPort);
    //for (processCt=0; processCt < 10; processCt++)
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            DieWithError("fork() failed");
        else if (processID == 0)  /* If this is the child process */
            ProcessMain(servSock, work_time, q_len);

    exit(0);  /* The children will carry on */
}

void ProcessMain(int servSock, int work_time, int q_len)
{
    int clntSock;                  /* Socket descriptor for client connection */

    for (;;)  /* Run forever */
    {
        // clntSock = AcceptTCPConnection(servSock);
        printf("with child process: %d\n", (unsigned int) getpid());
        HandleTCPClient(servSock, work_time, q_len);
    }
}
