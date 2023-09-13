#ifndef SEMA_WRAPPER_API
#define SEMA_WRAPPER_API

#define KEY int
#define SEMA_ID int

SEMA_ID OS_Create_Sema(KEY keyNum);
SEMA_ID OS_Get_Sema(KEY keyNum);
void OS_Lock_Sema(SEMA_ID id);
void OS_Release_Sema(SEMA_ID id);
void OS_Delete_Sema(SEMA_ID id);


#endif