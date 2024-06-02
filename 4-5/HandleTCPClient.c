#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */

int recvMsgSize;
struct sockaddr_in echoClntAddr; /* Client address */
unsigned int cliAddrLen;         /* Length of incoming message */


int queue_length;
int queue[1000];
int last = 0;
pthread_mutex_t file_mutex;



char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
int recvMsgSize;             /* Size of received message */

void clear_echo_buff() {
  for (int i = 0; i < RCVBUFSIZE; i++) {
    echoBuffer[i] = '\0';
  }
}

void getRequests(int sock) {
  clear_echo_buff();
  if ((recvMsgSize = recvfrom(sock, echoBuffer, RCVBUFSIZE, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
    DieWithError("recvfrom() failed");
  
  int fd;
  if((fd = open("queue.txt", O_WRONLY | O_APPEND, 0666)) < 0){
    DieWithError("Can\'t open file\n");
    
  }
  pthread_mutex_lock(&file_mutex);
  ssize_t  write_bytes = write(fd, echoBuffer, strlen(echoBuffer));
  pthread_mutex_unlock(&file_mutex);
  if(close(fd) < 0) {
      DieWithError("Can\'t close reading file\n");
  }
  if(write_bytes == -1) {
    DieWithError("Can\'t write this file\n");
  }
  
  
}

void fill_queue(int sock) {
  int fd;
  ssize_t read_bytes; 
  if((fd = open("queue.txt", O_RDONLY)) < 0){
      DieWithError("Can\'t open file\n");
    exit(-1);
  }
  char tmp[RCVBUFSIZE];
  int c = 0;
  
  pthread_mutex_lock(&file_mutex);
  do {
    clear_echo_buff();
    read_bytes = read(fd, echoBuffer, RCVBUFSIZE);
    if(read_bytes == -1) {
        DieWithError("Can\'t read this file\n");
      exit(-1);
    }
    
    //printf("%s\n", echoBuffer);
    
    for (size_t i = 0; i < strlen(echoBuffer); i++) {
      //printf("%d\n", last);
      if (echoBuffer[i] == '|') {
        tmp[c] = '\0';
        if (last < queue_length){
          queue[last++] = atoi(tmp); 
        } else { //выгоняем всех кто не попал в очередь
          printf("queue is full, client %s left\n", tmp);
          tmp[c] = '-';
          tmp[c+1] = '\0';
          if (sendto(sock, tmp, strlen(tmp), 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != strlen(tmp))
            DieWithError("send() failed");
        }
        c = 0;
        
      } else {
        tmp[c++] = echoBuffer[i];
      }
    }
  }while(read_bytes == RCVBUFSIZE);
  
  if(close(fd) < 0) {
    printf("Can\'t close reading file\n");
  }
  int fd2;
  
  if((fd2 = open("queue.txt", O_WRONLY)) < 0){
    printf("Can\'t open file\n");
    exit(-1);
  }

  int t = ftruncate(fd2, 0);
  
  pthread_mutex_unlock(&file_mutex);
  if(close(fd2) < 0) {
    printf("Can\'t close reading file\n");
  }
}

int localServSoc;
pid_t req_reader;
pid_t mainProc;

void my_handler(int nsig) {
  if (localServSoc != -1) {
    close(localServSoc);
  }
  kill(req_reader, SIGKILL);  
  kill(mainProc, SIGKILL);

  exit(0);
}

void onForever() {
  (void)signal(SIGINT, my_handler);
  for(;;);
}


void HandleTCPClient(int sock, int workTime, int q_len) {
  printf("woke up\n");
  queue_length = q_len;
  localServSoc = sock;
  
  int fd2;
  if((fd2 = open("queue.txt", O_WRONLY)) < 0){
    printf("Can\'t open file\n");
    exit(-1);
  }
  int tmp = ftruncate(fd2, 0);
  if(close(fd2) < 0) {
    printf("Can\'t close writing file\n");
  }
  
  req_reader = fork();
  if (req_reader == 0) {
    while (1){
      getRequests(sock);
    }
    exit(0);
  }

  mainProc = getpid();
  int c = fork();
  if (c == 0) {
    onForever();
  }
  
  
  while (1) /* zero indicates end of transmission */
  {
    cliAddrLen = sizeof(echoClntAddr);
    fill_queue(sock);
      // printf("%d\n", last);
      while (last > 0) {
        fill_queue(sock);
        printf("%d\n", last);
        printf("workig with client %d\n", queue[last-1]);
        sleep(workTime);
        sprintf(echoBuffer, "%d", queue[last-1]);
        int l = strlen(echoBuffer);
        echoBuffer[l] = '|';
        echoBuffer[l+1] = '\0';
        if (sendto(sock, echoBuffer, strlen(echoBuffer), 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != strlen(echoBuffer))
          DieWithError("send() failed");
        printf("done with current client\n");
        last--;
      }
    /* Echo message back to client */
    
    for (int j = 0; j < RCVBUFSIZE; j++) {echoBuffer[j] = '\0';}
    // printf("buffer %s \n", echoBuffer);
    /* See if there is more data to receive */
    if ((recvMsgSize = recvfrom(sock, echoBuffer, RCVBUFSIZE, 0,
      (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
      DieWithError("recvfrom() failed");
      
  }
  printf("going to sleep\n");
}
