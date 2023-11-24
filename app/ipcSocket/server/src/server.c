#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "remoteHaShaHandler.h"
#include "verifyLoginHandler.h"

#define PORT 8080

void loginHandling(void* iChannel);

int main(void)
{
    int iServerDes, iNewChanDes;
    struct sockaddr_in address;

    // init server socket 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if( !ServerInit(&iServerDes, address) ) {
        exit(EXIT_FAILURE);
    }
    
    printf("Set up success\n");
    printf("Wait for client connect ...\n");

    while(1) {
        // wait for client connect
        if( !WaitForClientConnect(iServerDes, address, &iNewChanDes) ) {
            perror("connection failed! Retry ...");
        }

        pthread_t sniffer_thread;
        if(pthread_create(&sniffer_thread, NULL, loginHandling, (void*) &iNewChanDes) < 0) {
            perror("pthread_create failed!");
            exit(EXIT_FAILURE); 
        }
    }

    // Close the listening socket
    shutdown(iServerDes, SHUT_RDWR);
    return 0;
}

void loginHandling(void* iChannel)
{
    // read message from client
    printf("Client connected! Wait for login authentication ...\n");
    loginMsg recMsg = {};
    if( !ReadLoginMSG(iChannel, &recMsg) ) {
        exit(EXIT_FAILURE);
    }

    // read login authentication done - verify it
    char* pcResMsg;
    if( verifyPassword(recMsg.pcUserLogin, recMsg.pcUserPsw) )
        pcResMsg = "Login succeed!";
    else
        pcResMsg = "Login failed";

    // send message to server
    if(send(iChannel, pcResMsg, strlen(pcResMsg), 0) < 0) {
        perror("send response msg failed");
    }

    /* close socket and clean up */
	close(iChannel);
	pthread_exit(0);
}