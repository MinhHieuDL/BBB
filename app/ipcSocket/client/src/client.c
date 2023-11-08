#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "remoteHaShaHandler.h" 
#define PORT 8080 
  
int main(int argc, char const* argv[]) 
{ 
    int client_fd; 
    struct sockaddr_in serv_addr; 
    char* hello = "Hello from client"; 
    char buffer[1024] = { 0 }; 
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("Socket creation error \n"); 
        exit(EXIT_FAILURE); 
    } 
  
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
  
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if (inet_pton(AF_INET, "192.168.1.29", &serv_addr.sin_addr) <= 0) { 
        printf("Invalid address -- Address not supported \n"); 
        exit(EXIT_FAILURE); 
    } 
  
    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { 
        perror("connect Failed"); 
        exit(EXIT_FAILURE); 
    }

    // send data to server 
    if(send(client_fd, hello, strlen(hello), 0) < 0) {
        perror("send failed");
    }
    printf("Send message to server successed \n");
    
    // read message from server
    if(read(client_fd, buffer, 1024) < 0) {
        perror("read failed");
    } 
    else {
        printf("Message from server received: %s \n", buffer);
    }
  
    // closing the connected socket 
    close(client_fd); 
    return 0; 
}
