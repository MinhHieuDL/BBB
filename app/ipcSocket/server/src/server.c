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

// Global variable to track if termination signal received
volatile sig_atomic_t terminate = 0;

void sigint_handler(int signum);
void* loginHandling(void* iChannel);

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
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(iServerDes, &readfds);

    while(!terminate) {
        int activity = select(iServerDes + 1, &readfds, NULL, NULL, &timeout);

        if(activity == -1) {
            perror("select() failed");
            exit(EXIT_FAILURE);
        } else if (activity == 0) {
            // no incomming request during 'timeout', continue to check if 
            // any terminal request from user
            continue;
        }

        printf("Request received\n");

        // iServerDes ready for reading, accept the request and
        // create thread to handling client 
        if( !WaitForClientConnect(iServerDes, address, &iNewChanDes) ) {
            perror("connection failed! Retry ...");
        } else {
            pthread_t sniffer_thread;
            if(pthread_create(&sniffer_thread, NULL, loginHandling, (void*) &iNewChanDes) < 0) {
                perror("pthread_create failed!");
                exit(EXIT_FAILURE); 
            }
        }
    }

    // Close the listening socket
    shutdown(iServerDes, SHUT_RDWR);
    return 0;
}

void* loginHandling(void* iChannel)
{
    // read message from client
    printf("Client connected! Wait for login authentication ...\n");
    loginMsg recMsg = {};
    if( !ReadLoginMSG(*((int*)iChannel), &recMsg) ) {
        exit(EXIT_FAILURE);
    }

    // read login authentication done - verify it
    char* pcResMsg;
    if( verifyPassword(recMsg.pcUserLogin, recMsg.pcUserPsw) )
        pcResMsg = "Login succeed!";
    else
        pcResMsg = "Login failed";

    // send message to server
    if(send(*((int*)iChannel), pcResMsg, strlen(pcResMsg), 0) < 0) {
        perror("send response msg failed");
    }

    /* close socket and clean up */
	close(*((int*)iChannel));
	pthread_exit(0);
}

void sigint_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nTermination signal received. Shutting down the server...\n");
        terminate = 1;
    }
}