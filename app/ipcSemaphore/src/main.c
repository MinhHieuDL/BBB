#include "semaWrapper.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SEM_KEY 29

void cleanup(int signal); // Declaration

int main(void)
{
    int semID, fd;
    pid_t child;

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
            sleep(2);
            printf("Child signal sema\n");
            if(OS_Sema_Signal(SEM_KEY) < 0)
                exit(1);
        }
    }
    else {
        // Handle terminate signal
        if(signal(SIGINT, cleanup) == SIG_ERR)
        {
            perror("signal");
            return -1;
        }
        while(1){
            printf("Parent Run\n");
            OS_Sema_Wait(SEM_KEY);
            printf("Parent write to file\n");
            write(fd, "a ", 2);
            sleep(2);
            printf("Parent signal sema\n");
            OS_Sema_Signal(SEM_KEY);
        }
    }

    return 0;
}

void cleanup(int signal)
{
    // Delete sema
    printf("App died: Delete Semaphore key num: %d\n", SEM_KEY);
    OS_Sema_Del(SEM_KEY);
    exit(0);
}