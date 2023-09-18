#include "semaWrapper.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SEM_KEY 29

void cleanup(int signal); // Declaration
int semID;
int main(void)
{
    int fd;
    pid_t child;

    // Handle terminate signal
    if(signal(SIGINT, cleanup) == SIG_ERR)
    {
        perror("signal");
        return -1;
    }
    // Create semaphore
    semID = OS_Sema_Create(SEM_KEY); 
    if(semID == -1)
    {
        printf("Create semaphore failed\n");
        exit(1);
    }
    // Open test file
    fd = open("test.txt", O_RDWR);
    if(fd < 0)
    {
        perror("open file");
        exit(1);
    }
    // Create scenario where 2 process access to a text file
    child = fork();
    if(child == 0) {
        while(1){
            printf("Child Run\n");
            if(OS_Sema_Wait(SEM_KEY) < 0){
                exit(1);
            }
            printf("Child write to file\n");
            write(fd, "b ", 2);
            if(OS_Sema_Signal(SEM_KEY) < 0)
                exit(1);
            sleep(2);
        }
    }
    else {
        while(1){
            printf("Parent Run\n");
            OS_Sema_Wait(SEM_KEY);
            printf("Parent write to file\n");
            write(fd, "a ", 2);
            OS_Sema_Signal(SEM_KEY);
            sleep(2);
        }
    }

    return 0;
}

void cleanup(int signal)
{
    // Delete sema
    printf("App died: Delete Semaphore\n");
    OS_Sema_Del(semID);
    exit(0);
}