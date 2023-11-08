#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "remoteHaShaHandler.h"
#include "verifyLoginHandler.h"

#define PORT 8080

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

    // wait for client connect
    if( !WaitForClientConnect(iServerDes, address, &iNewChanDes) ) {
        exit(EXIT_FAILURE);
    }
    
    if ((iNewChanDes = accept(iServerDes, (struct sockaddr*)&address, (socklen_t*)sizeof(address))) < 0) {
        exit(EXIT_FAILURE);
    }

    // read message from client
    printf("Client connected! Wait for login authentication ...\n");
    loginMsg recMsg;
    if( !ReadLoginMSG(iNewChanDes, &recMsg) ) {
        exit(EXIT_FAILURE);
    }

    // read login authentication done - verify it
    char* pcResMsg;
    if( verifyPassword(recMsg.pcUserLogin, recMsg.pcUserPsw) )
        pcResMsg = "Login succeed!";
    else
        pcResMsg = "Login failed";

    // send message to server
    if(send(iNewChanDes, pcResMsg, strlen(pcResMsg), 0) < 0) {
        perror("send response msg failed");
    }   
    
    // Clean up resource
    // Close new socket channel
    close(iNewChanDes);
    // Close the listening socket
    shutdown(iServerDes, SHUT_RDWR);
    return 0;
}