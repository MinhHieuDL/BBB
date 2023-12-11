#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include "remoteHaShaHandler.h"
#include "verifyLoginHandler.h"

#define PORT 8080

typedef struct loginArg{
    int iChanel;
    char ipAddr[INET6_ADDRSTRLEN];
}loginArg;

// Global variable to track if termination signal received
volatile sig_atomic_t terminate = 0;

void sigint_handler(int signum);
void* loginHandling(void* arg);

int main(void)
{
    int iServerDes, iNewChanDes;
    struct sockaddr_in address;
    
    // Register signal handler for Ctrl + C (SIGINT)
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal failed");
        exit(EXIT_FAILURE);
    }

    // init server socket 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if( !ServerInit(&iServerDes, address) ) {
        exit(EXIT_FAILURE);
    }
    
    printf("Set up success\n");
    printf("Wait for client connect ...\n");

    // Set up to check the readiness of server file descriptor
    struct timeval timeout;
    // Create file descriptor set
    fd_set readfds;
    
    while(!terminate) {
        // set to track server file description  
        FD_ZERO(&readfds);
        FD_SET(iServerDes, &readfds);
        // set timeout to keep track
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
    
        int activity = select(iServerDes + 1, &readfds, NULL, NULL, &timeout);

        if(activity == -1) {
            perror("select() failed");
            exit(EXIT_FAILURE);
        } else if (activity == 0) {
            // printf("No incomming request - time: %u\n", (unsigned)time(NULL));
            continue;
        }

        // iServerDes ready for reading, accept the request and
        // create thread to handling client
        struct sockaddr_in clientAddress; 
        if( !WaitForClientConnect(iServerDes, &clientAddress, &iNewChanDes) ) {
            perror("connection failed! Retry ...");
        } else {
            // print out the client ip address
            loginArg* pThreadArg = (loginArg*)malloc(sizeof(loginArg));
            inet_ntop(AF_INET, &clientAddress.sin_addr, pThreadArg->ipAddr, sizeof(pThreadArg->ipAddr));
            printf("Client with ip: %s connected\n", pThreadArg->ipAddr);
            // create thread to handle client request
            pThreadArg->iChanel = iNewChanDes;
            pthread_t sniffer_thread;
            if(pthread_create(&sniffer_thread, NULL, loginHandling, (void*) pThreadArg) < 0) {
                perror("pthread_create failed!");
                exit(EXIT_FAILURE); 
            }
        }
    }

    // Close the listening socket
    shutdown(iServerDes, SHUT_RDWR);
    return 0;
}

void* loginHandling(void* arg)
{
    loginArg* localArg = (loginArg*)arg;
    // read message from client
    fflush(stdout);
    printf("Wait for login authentication ...\n");
    loginMsg recMsg = {};
    if( !ReadLoginMSG(localArg->iChanel, &recMsg) ) {
        exit(EXIT_FAILURE);
    }

    // read login authentication done - verify it
    char* pcResMsg;
    if( verifyPassword(recMsg.pcUserLogin, recMsg.pcUserPsw) )
        pcResMsg = "Login succeed!";
    else
        pcResMsg = "Login failed";

    // send message to server
    if(send(localArg->iChanel, pcResMsg, strlen(pcResMsg), 0) < 0) {
        printf("send data to client with ip: %s failed\n", localArg->ipAddr);
        perror("send response msg failed");
    }
    
    // print status of client login
    printf("client with ip: %s - %s with user: %s\n", localArg->ipAddr, pcResMsg, recMsg.pcUserLogin);

    /* close socket and clean up */
	close(localArg->iChanel);
    free(arg);
	pthread_exit(0);
}

void sigint_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nTermination signal received. Shutting down the server...\n");
        terminate = 1;
    }
}