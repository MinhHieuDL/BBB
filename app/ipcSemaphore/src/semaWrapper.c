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

int OS_Sema_Wait(key_t keyNum)
{
    int semID, iRet;
    // create ops for V
    struct sembuf vsembuf ={
        .sem_num = 0,
        .sem_op = -1,
        .sem_flg = SEM_UNDO
    };
    // Get semaphore
    semID = semget(keyNum, 1, 0777);
    if (semID == -1){
        perror("semget");
        return -1;
    }
    // Lock semaphore
    iRet = semop(semID, &vsembuf, 1);
    if(iRet < 0)
    {
        perror("semop - wait:");
    }
    return iRet;
}

int OS_Sema_Signal(key_t keyNum)
{
    int semID, iRet;
    // create ops for P
    struct sembuf psembuf ={
        .sem_num = 0,
        .sem_op = 1,
        .sem_flg = SEM_UNDO
    };
    // Get semaphore
    semID = semget(keyNum, 1, 0777);
    if (semID == -1){
        perror("semget");
        return -1;
    }
    // Wake up process wait for semaphore increase
    iRet = semop(semID, &psembuf, 1);
    if(iRet < 0)
    {
        perror("semop - signal:");
    }
    return iRet;
}

int OS_Sema_Del(key_t keyNum)
{
    int semID;
    // Get semaphore
    semID = semget(keyNum, 1, 0777);
    if (semID == -1){
        perror("semget");
        return -1;
    }
    // Delete semaphore
    if(semctl(semID, 1, IPC_RMID, 0) == -1){
        perror("semctl - IPC_RMID");
        return -1;
    }

    return 0;
}