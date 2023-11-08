#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "remoteHaShaHandler.h" 
#define PORT 8080 
  
int main(int argc, char const* argv[]) 
{ 
    int client_fd; 
    
    // init client socket
    if( !ClientInit(&client_fd) ) {
        exit(EXIT_FAILURE);
    }
    
    // Connect to server
    char stAdd[20];
    printf("enter server address to connect: ");
    fgets(stAdd, sizeof(stAdd), stdin);
    
    printf("wait for connecting ....\n");
    if ( !ServerConnect(client_fd, stAdd, PORT) )
    {
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Finded the server - enter login authentication \n");

    // Send login authentication
    char user[30];
    char psswd[30];
    printf("user: ");
    fgets(user, sizeof(user), stdin);
    printf("psswd: ");
    fgets(psswd, sizeof(psswd), stdin);

    // send data to server
    loginMsg sendMsg = {
        .pcUserLogin = user,
        .pcUserPsw = psswd
    };

    if( !SendLoginMsg(client_fd, sendMsg) ) {
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    // read message from server
    char readBuff[1024];
    if(read(client_fd, readBuff, 1024) < 0) {
        perror("read failed");
    } 
    else {
        printf("login status: %s \n", readBuff);
    }
  
    // closing the connected socket 
    close(client_fd); 
    return 0; 
}
