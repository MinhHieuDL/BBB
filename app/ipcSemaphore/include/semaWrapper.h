#ifndef SEMA_WRAPPER_API
#define SEMA_WRAPPER_API

#include <sys/types.h>
#include <sys/sem.h>
#include <stdio.h>

typedef int _SEMA_ID;

_SEMA_ID OS_Sema_Create(key_t keyNum);
int OS_Sema_Wait(key_t keyNum);
int OS_Sema_Signal(key_t keyNum);
int OS_Sema_Del(key_t keyNum);


#endif