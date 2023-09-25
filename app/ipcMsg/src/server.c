#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "msgData.h"

#define MSGKEY 29

int msgid;

void cleanup(int signal); // Declaration

int main(void)
{    
    int pid, *pData;
    msgform msg;

    // Handle terminate signal
    if(signal(SIGINT, cleanup) == SIG_ERR)
    {
        perror("signal");
        return -1;
    }

    msgid = msgget(MSGKEY, 0777 | IPC_CREAT);

    while(1)
    {
        msgrcv(msgid, &msg, 256, 1, 0);
        pData = (int*) msg.m_text;
        pid = *pData;
        printf("Server: received data from client pid: %d\n", pid); 
        msg.m_ltype = pid;
        *pData = getpid();
        msgsnd(msgid, &msg, sizeof(int), 0);
    }

    return 0;
}

void cleanup(int signal)
{
    printf("Server terminated: cleaning up msg queue\n"); 
    msgctl(msgid, IPC_RMID, 0);
    exit(0);
}
