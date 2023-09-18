#include "semaWrapper.h"
#include <stdlib.h>

#define SEM_KEY 29

int main(void)
{
    int semID;
    
    semID = OS_Sema_Create(SEM_KEY); 

    // Create success
    if(semID == -1){
        printf("Create sema failed\n");
        exit(1);       
    }

    // Delete sema
    if(OS_Sema_Del(semID) == -1){
        printf("Delete sema failed\n");
        exit(1);
    }

    return 0;
}