#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "msgData.h"

#define MSGKEY 29

int main(void)
{
    msgform msg;
    int msgid, pid, *pData;
    
    msgid = msgget(MSGKEY, 0777);
    
    pid = getpid();
    pData = (int*)msg.m_text;
    *pData = pid; // copy pid into message text 
    msg.m_ltype = 1 ;
    
    msgsnd (msgid, &msg, sizeof(int) , 0) ;
    msgrcv (msgid, &msg, 256, pid, 0) ; // pid is used as the msg type
    printf("client: receive from pid %d\n", *pData) ;
    return 0;
}
