#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

int main(void)
{
    int iServerDes, iNewChanDes;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char* hello = "Hello from server"; 

    // Create socket file description 
    if((iServerDes = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set option for able to reuse the address
    if (setsockopt(iServerDes, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080 
    if (bind(iServerDes, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if(listen(iServerDes, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Set up success\n");
    printf("Wait for client connect ...\n");
    
    if ((iNewChanDes = accept(iServerDes, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int valread = read(iNewChanDes, buffer, 1024);
    printf("valread: %d\n", valread);
    printf("Message from client: %s\n", buffer);
    // int sendRet = send(iNewChanDes, hello, strlen(hello), 0); 
    // printf("Hello message sent - sendRet: %d\n", sendRet); 
    // if(sendRet < 0)
    //     perror("send failed");
    // Clean up resource
    // Close new socket channel
    close(iNewChanDes);
    // Close the listening socket
    shutdown(iServerDes, SHUT_RDWR);
    return 0;
}