#include <arpa/inet.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#define PORT 8080 
  
int main(int argc, char const* argv[]) 
{ 
    int status, valread, client_fd; 
    struct sockaddr_in serv_addr; 
    char* hello = "Hello from client"; 
    char buffer[1024] = { 0 }; 
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
  
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
  
    // Convert IPv4 and IPv6 addresses from text to binary 
    // form 
    if (inet_pton(AF_INET, "192.168.1.29", &serv_addr.sin_addr) 
        <= 0) { 
        printf( 
            "\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
  
    if ((status 
         = connect(client_fd, (struct sockaddr*)&serv_addr, 
                   sizeof(serv_addr))) 
        < 0) { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    int sendRet = send(client_fd, hello, strlen(hello), 0); 
    printf("Hello message sent - sendRet: %d\n", sendRet);
    if(sendRet < 0)
        perror("send failed"); 
    // valread = read(client_fd, buffer, 1024); 
    // printf("valread: %d\n", valread);
    // if(valread < 0)
    //     perror("read failed");
    // printf("%s\n", buffer); 
  
    // closing the connected socket 
    close(client_fd); 
    return 0; 
}