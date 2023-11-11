#include "userDBHandler.h"
#include "encodeHandler.h"

#define MAX_USER_SIZE   30
#define MAX_PASSWORD_SIZE   30

//************* define api *********************


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
                // Request user and psw from user
                char user[MAX_USER_SIZE];
                char passwd[MAX_PASSWORD_SIZE];
                printf("user: ");
                scanf(" %s", user);
                printf("password: ");
                scanf(" %s", passwd);

                int iHashSize = getHashSize();
                unsigned char hash[iHashSize];
                if(encodePsw(passwd, hash))
                {
                    char newLineDB[MAX_USER_SIZE + iHashSize * 2 + 1 + 1]; // +1 for ',' and +1 for null terminator
                    memset(newLineDB, 0, sizeof(newLineDB)); 
                    sprintf(newLineDB, "%s,", user);
                    for (unsigned int i = 0; i < iHashSize; i++) 
                    {
                        sprintf(newLineDB + strlen(newLineDB), "%02x", hash[i]);
                    }
                    printf("Hashed Password: %s\n", newLineDB);
                    addNewLine(pcDBFile, newLineDB);
                }
                else 
                    printf("encode password failed, please check and update again\n");
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