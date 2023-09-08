#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int data[32];

int main(void)
{
    char *c = (char*)calloc(100, sizeof(char));
    int fd, sz;
    fd = open("test.txt", O_RDONLY | O_CREAT);
    if(fd < 0)
    {
       perror("open test.txt failed");
       exit(1);
    }
    else 
        sz = read(fd, c, 20);
    c[sz] = '\0';
    printf("content read from text file: %s\n",c);
    close(fd);
    return 0;
}