#include <sys/ipc.h>
#include "semaWrapper.h"

_SEMA_ID OS_Sema_Create(key_t keyNum)
{
    
    return 0;
}

_SEMA_ID OS_Sema_Wait(key_t keyNum)
{
    return 0;
}

void OS_Sema_Signal(_SEMA_ID id)
{

}

void OS_Sema_Del(_SEMA_ID id)
{
    
}