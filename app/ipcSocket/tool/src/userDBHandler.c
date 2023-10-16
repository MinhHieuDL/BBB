#include "userDBHandler.h"

void updateDB(const char* pcDBFile)
{
    char userCMD;
    printf("\n---------------------------------------\nUpdate mode:\n"
            "a - add user and password to database\n"
            "b - back to main mode\n");

    while(1)
    {
        printf("update mode - enter command: ");
        scanf(" %c", &userCMD);
        
        if(userCMD == 'b')
            break;
        
        switch (userCMD)
        {
            case 'a':
            {
                char user[30];
                char passwd[30];
                char newLineDB[60];
                printf("user: ");
                scanf(" %s", user);
                printf("password: ");
                scanf(" %s", passwd);
                snprintf(newLineDB,sizeof(newLineDB), "%s,%s", user, passwd);
                addNewLine(pcDBFile, newLineDB);
            }    
            break;

            default:
                printf("Update mode, invalid cmd: %c\n", userCMD);
            break;
        }
        getchar();
    }
    printf("Back to main mode\n");
    return;
}