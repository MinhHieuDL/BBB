#include <sys/ipc.h>
#include "semaWrapper.h"

_SEMA_ID OS_Sema_Create(key_t keyNum)
{
    int iSemaID;
    unsigned short usInitValue[1];
    
    // Create new sema
    iSemaID = semget(keyNum, 1, 0777|IPC_CREAT);
    if (iSemaID == -1) {
        perror("semget");
        return -1;
    }
    // Init sema value to 1 
    usInitValue[0] = 1;
    if (semctl(iSemaID, 1, SETALL, usInitValue) == -1){
        perror("semctl - SETALL");
        return -1;
    }
    
    return iSemaID;
}

_SEMA_ID OS_Sema_Wait(key_t keyNum)
{
    return 0;
}

void OS_Sema_Signal(_SEMA_ID id)
{

}

int OS_Sema_Del(_SEMA_ID id)
{
    if(semctl(id, 1, IPC_RMID, 0) == -1){
        perror("semctl - IPC_RMID");
        return -1;
    }

    return 0;
}