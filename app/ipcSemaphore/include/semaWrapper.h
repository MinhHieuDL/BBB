#ifndef SEMA_WRAPPER_API
#define SEMA_WRAPPER_API

#include <sys/types.h>
#include <sys/sem.h>
#include <stdio.h>

typedef int _SEMA_ID;

_SEMA_ID OS_Sema_Create(key_t keyNum);
_SEMA_ID OS_Sema_Wait(key_t keyNum);
void OS_Sema_Signal(_SEMA_ID id);
int OS_Sema_Del(_SEMA_ID id);


#endif